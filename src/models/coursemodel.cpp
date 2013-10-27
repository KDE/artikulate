/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License or (at your option) version 3 or any later version
 *  accepted by the membership of KDE e.V. (or its successor approved
 *  by the membership of KDE e.V.), which shall act as a proxy
 *  defined in Section 14 of version 3 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "coursemodel.h"
#include "core/language.h"
#include "core/course.h"
#include "core/resourcemanager.h"
#include "core/resources/courseresource.h"

#include <QAbstractListModel>
#include <QSignalMapper>

#include <KLocale>
#include <KDebug>

CourseModel::CourseModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_resourceManager(0)
    , m_language(0)
    , m_view(OnlyGetHotNewStuffResources)
    , m_signalMapper(new QSignalMapper(this))
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[DescriptionRole] = "description";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";
    setRoleNames(roles);

    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitCourseChanged(int)));
}

void CourseModel::setResourceManager(ResourceManager *resourceManager)
{
    if (m_resourceManager == resourceManager) {
        return;
    }

    beginResetModel();

    if (m_resourceManager) {
        m_resourceManager->disconnect(this);
    }

    m_resourceManager = resourceManager;

    if (m_resourceManager) {
        connect(m_resourceManager, SIGNAL(courseResourceAboutToBeAdded(CourseResource*,int)),
                SLOT(onCourseResourceAboutToBeAdded(CourseResource*,int)));
        connect(m_resourceManager, SIGNAL(courseResourceAdded()),
                SLOT(onCourseResourceAdded()));
        connect(m_resourceManager, SIGNAL(courseResourceAboutToBeRemoved(int)),
                SLOT(onCourseResourceAboutToBeRemoved(int)));
        updateResources();
    }

    endResetModel();

    emit resourceManagerChanged();
}

ResourceManager * CourseModel::resourceManager() const
{
    return m_resourceManager;
}

Language * CourseModel::language() const
{
    return m_language;
}

void CourseModel::setLanguage(Language *language)
{
    if (!language) {
        return;
    }
    emit beginResetModel();
    m_language = language;
    updateResources();
    emit languageChanged();
    emit endResetModel();
}

CourseModel::CourseResourceView CourseModel::view() const
{
    return m_view;
}

void CourseModel::setView(CourseModel::CourseResourceView view)
{
    if (m_view == view) {
        return;
    }
    emit beginResetModel();
    m_view = view;
    updateResources();
    emit viewChanged();
    emit endResetModel();
}

QVariant CourseModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_resources.count()) {
        return QVariant();
    }

    Course * const course = m_resources.at(index.row())->course();

    switch(role)
    {
    case Qt::DisplayRole:
        return !course->title().isEmpty()?
                QVariant(course->title()): QVariant(i18nc("@item:inlistbox:", "unknown"));
    case Qt::ToolTipRole:
        return QVariant(course->title());
    case TitleRole:
        return course->title();
    case DescriptionRole:
        return course->description();
    case IdRole:
        return course->id();
    case DataRole:
        return QVariant::fromValue<QObject*>(course);
    default:
        return QVariant();
    }
}

int CourseModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_resources.count();
}

void CourseModel::onCourseResourceAboutToBeAdded(CourseResource *resource, int index)
{
    // do nothing when view does not show resource
    if (resource->isContributorResource() == true && m_view == OnlyGetHotNewStuffResources) {
        return;
    }
    if (!resource->isContributorResource() == true && m_view == OnlyContributorResources) {
        return;
    }

    beginInsertRows(QModelIndex(), m_resources.count(), m_resources.count());
    m_resources.append(resource);

    connect(resource->course(), SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    //TODO add missing signals
}

void CourseModel::onCourseResourceAdded()
{
    updateMappings();
    endInsertRows();
}

void CourseModel::onCourseResourceAboutToBeRemoved(int index)
{
    if (!m_language) {
        return;
    }
    CourseResource *originalResource = m_resourceManager->courseResources(m_language).at(index);
    int modelIndex = m_resources.indexOf(originalResource);

    if (modelIndex == -1) {
        kWarning() << "Cannot remove course from model, not registered";
        return;
    }
    beginRemoveRows(QModelIndex(), modelIndex, modelIndex);
    m_resources.removeAt(modelIndex);
    endRemoveRows();
}

void CourseModel::emitCourseChanged(int row)
{
    emit courseChanged(row);
    emit dataChanged(index(row, 0), index(row, 0));
}

QVariant CourseModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    return QVariant(i18nc("@title:column", "Course"));
}

void CourseModel::updateResources()
{
    if (!m_resourceManager) {
        return;
    }
    if (!m_language) {
        return;
    }
    m_resources.clear();
    QList<CourseResource*> resources = m_resourceManager->courseResources(m_language);
    switch (m_view) {
    case CourseModel::AllResources:
        m_resources = resources;
        break;
    case CourseModel::OnlyContributorResources:
        foreach(CourseResource *resource, resources) {
            if (!resource->isContributorResource()) {
                continue;
            }
            m_resources.append(resource);
        }
        break;
    case CourseModel::OnlyGetHotNewStuffResources:
        foreach(CourseResource *resource, resources) {
            if (resource->isContributorResource()) {
                continue;
            }
            m_resources.append(resource);
        }
        break;
    }
    updateMappings();
}

void CourseModel::updateMappings()
{
    if (!m_language) {
        kDebug() << "Aborting to update mappings, language not set.";
        return;
    }
    int courses = m_resources.count();
    for (int i = 0; i < courses; i++) {
        m_signalMapper->setMapping(m_resources.at(i)->course(), i);
    }
}
