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
#include <QDebug>
#include <QSignalMapper>
#include <KLocalizedString>

CourseModel::CourseModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_resourceManager(nullptr)
    , m_language(nullptr)
    , m_signalMapper(new QSignalMapper(this))
{
    connect(m_signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped),
            this, &CourseModel::emitCourseChanged);
    connect(this, &CourseModel::resourceManagerChanged,
            this, &CourseModel::rowCountChanged);
    connect(this, &CourseModel::languageChanged,
            this, &CourseModel::rowCountChanged);
}

CourseModel::~CourseModel()
{

}

QHash< int, QByteArray > CourseModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[DescriptionRole] = "description";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";

    return roles;
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
    m_resources.clear();
    if (m_resourceManager) {
        connect(m_resourceManager, SIGNAL(courseResourceAboutToBeAdded(CourseResource*,int)),
                SLOT(onCourseResourceAboutToBeAdded(CourseResource*,int)));
        connect(m_resourceManager, SIGNAL(courseResourceAdded()),
                SLOT(onCourseResourceAdded()));
        connect(m_resourceManager, SIGNAL(courseResourceAboutToBeRemoved(int)),
                SLOT(onCourseResourceAboutToBeRemoved(int)));
    }
    if (m_language && m_resourceManager) {
        m_resources = m_resourceManager->courseResources(m_language);
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
    emit beginResetModel();
    m_language = language;
    m_resources.clear();
    if (m_language) {
        m_resources = m_resourceManager->courseResources(m_language);
    }
    emit languageChanged();
    emit endResetModel();
    emit rowCountChanged();
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
    case ContributerResourceRole:
        return m_resources.at(index.row())->isContributorResource();
    case DataRole:
        return QVariant::fromValue<QObject*>(course);
    default:
        return QVariant();
    }
}

int CourseModel::rowCount(const QModelIndex& parent) const
{
    if (!m_language) {
        return 0;
    }
    if (parent.isValid()) {
        return 0;
    }
    return m_resources.count();
}

void CourseModel::onCourseResourceAboutToBeAdded(CourseResource *resource, int index)
{
    beginInsertRows(QModelIndex(), m_resources.count(), m_resources.count());
    m_resources.append(resource);

    connect(resource->course(), SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    //TODO add missing signals
}

void CourseModel::onCourseResourceAdded()
{
    updateMappings();
    endInsertRows();
    emit rowCountChanged();
}

void CourseModel::onCourseResourceAboutToBeRemoved(int index)
{
    if (!m_language) {
        return;
    }
    CourseResource *originalResource = m_resourceManager->courseResources(m_language).at(index);
    int modelIndex = m_resources.indexOf(originalResource);

    if (modelIndex == -1) {
        qWarning() << "Cannot remove course from model, not registered";
        return;
    }
    beginRemoveRows(QModelIndex(), modelIndex, modelIndex);
    m_resources.removeAt(modelIndex);
    endRemoveRows();
    emit rowCountChanged();
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

void CourseModel::updateMappings()
{
    if (!m_language) {
        qDebug() << "Aborting to update mappings, language not set.";
        return;
    }
    int courses = m_resources.count();
    for (int i = 0; i < courses; i++) {
        m_signalMapper->setMapping(m_resources.at(i)->course(), i);
    }
}

QVariant CourseModel::course(int row) const
{
    return data(index(row, 0), CourseModel::DataRole);
}
