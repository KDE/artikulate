/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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

#include <QAbstractListModel>
#include <QSignalMapper>

#include <KLocale>
#include <KDebug>

CourseModel::CourseModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_resourceManager(0)
    , m_language(0)
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
        connect(m_resourceManager, SIGNAL(courseAboutToBeAdded(Course*,int)), SLOT(onCourseAboutToBeAdded(Course*,int)));
        connect(m_resourceManager, SIGNAL(courseAdded()), SLOT(onCourseAdded()));
        connect(m_resourceManager, SIGNAL(courseAboutToBeRemoved(int,int)), SLOT(onCoursesAboutToBeRemoved(int,int)));
        connect(m_resourceManager, SIGNAL(courseRemoved()), SLOT(onCoursesRemoved()));
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
    emit languageChanged();

    emit endResetModel();
}

QVariant CourseModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_resourceManager->courseList().count()) {
        return QVariant();
    }

    Course * const course = m_resourceManager->course(m_language, index.row());

    switch(role)
    {
    case Qt::DisplayRole:
        return !course->title().isEmpty()?
                QVariant(course->title()): QVariant(i18n("<No title>"));
    case Qt::ToolTipRole:
        return QVariant(i18n("<p>%1</p>", course->title()));
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
    if (!m_resourceManager) {
        return 0;
    }

    if (!m_language) {
        return 0;
    }

    if (parent.isValid()) {
        return 0;
    }

    return m_resourceManager->courseList(m_language).count();
}

void CourseModel::onCourseAboutToBeAdded(Course *course, int index)
{
    connect(course, SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    //TODO add missing signals
    beginInsertRows(QModelIndex(), index, index);
}

void CourseModel::onCourseAdded()
{
    updateMappings();
    endInsertRows();
}

void CourseModel::onCoursesAboutToBeRemoved(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void CourseModel::onCoursesRemoved()
{
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
    return QVariant(i18n("Title"));
}

void CourseModel::updateMappings()
{
    if (!m_language) {
        kDebug() << "Aborting to update mappings, language not set.";
        return;
    }
    int courses = m_resourceManager->courseList(m_language).count();
    for (int i = 0; i < courses; i++) {
        m_signalMapper->setMapping(m_resourceManager->course(m_language, i), i);
    }
}
