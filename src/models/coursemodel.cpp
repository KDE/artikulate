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
#include "core/iresourcerepository.h"
#include "core/resources/courseresource.h"
#include <QAbstractListModel>
#include "artikulate_debug.h"
#include <QSignalMapper>
#include <KLocalizedString>
#include "application.h"

CourseModel::CourseModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_resourceRepository(nullptr)
    , m_language(nullptr)
    , m_signalMapper(new QSignalMapper(this))
{
    connect(m_signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped),
            this, &CourseModel::emitCourseChanged);
    connect(this, &CourseModel::resourceManagerChanged,
            this, &CourseModel::rowCountChanged);
    connect(this, &CourseModel::languageChanged,
            this, &CourseModel::rowCountChanged);

    setResourceRepository(artikulateApp->resourceRepository());
}

QHash< int, QByteArray > CourseModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[DescriptionRole] = "description";
    roles[IdRole] = "id";
    roles[LanguageRole] = "language";
    roles[DataRole] = "dataRole";

    return roles;
}

void CourseModel::setResourceRepository(IResourceRepository *resourceRepository)
{
    if (resourceRepository == nullptr) {
        qCWarning(ARTIKULATE_CORE()) << "setting resource repository to nullptr, this shall never happen";
    }
    Q_ASSERT(resourceRepository != nullptr);

    if (m_resourceRepository == resourceRepository) {
        return;
    }

    beginResetModel();

    if (m_resourceRepository) {
        disconnect(m_resourceRepository, &IResourceRepository::courseAboutToBeAdded, this, &CourseModel::onCourseAboutToBeAdded);
        disconnect(m_resourceRepository, &IResourceRepository::courseAdded, this, &CourseModel::onCourseAdded);
        disconnect(m_resourceRepository, &IResourceRepository::courseAboutToBeRemoved, this, &CourseModel::onCourseAboutToBeRemoved);
    }

    m_resourceRepository = resourceRepository;
    m_courses.clear();
    if (m_resourceRepository) {
        connect(m_resourceRepository, &IResourceRepository::courseAboutToBeAdded, this, &CourseModel::onCourseAboutToBeAdded);
        connect(m_resourceRepository, &IResourceRepository::courseAdded, this, &CourseModel::onCourseAdded);
        connect(m_resourceRepository, &IResourceRepository::courseAboutToBeRemoved, this, &CourseModel::onCourseAboutToBeRemoved);
    }
    if (m_resourceRepository) {
        m_courses = m_resourceRepository->courses(m_language);
    }
    endResetModel();
    emit resourceManagerChanged();
}

IResourceRepository * CourseModel::resourceRepository() const
{
    return m_resourceRepository;
}

Language * CourseModel::language() const
{
    return m_language;
}

void CourseModel::setLanguage(Language *language)
{
    beginResetModel();
    m_language = language;
    m_courses = m_resourceRepository->courses(m_language);
    emit languageChanged();
    endResetModel();
    emit rowCountChanged();
}

QVariant CourseModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_courses.count()) {
        return QVariant();
    }

    ICourse * const course = m_courses.at(index.row());

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
        return false;// m_resources.at(index.row())->isContributorResource();//FIXME
    case LanguageRole:
        return QVariant::fromValue<QObject*>(course->language());
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
    return m_courses.count();
}

void CourseModel::onCourseAboutToBeAdded(ICourse *course, int index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), m_courses.count(), m_courses.count());
    m_courses.append(course);

    connect(course, SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    //TODO add missing signals
}

void CourseModel::onCourseAdded()
{
    updateMappings();
    endInsertRows();
    emit rowCountChanged();
}

void CourseModel::onCourseAboutToBeRemoved(int index)
{
    if (index >= m_resourceRepository->courses(m_language).count()) {
        return;
    }
    ICourse *originalCourse = m_resourceRepository->courses(m_language).at(index);
    int modelIndex = m_courses.indexOf(originalCourse);

    if (modelIndex == -1) {
        qCWarning(ARTIKULATE_LOG) << "Cannot remove course from model, not registered";
        return;
    }
    beginRemoveRows(QModelIndex(), modelIndex, modelIndex);
    m_courses.removeAt(modelIndex);
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
    int courses = m_courses.count();
    for (int i = 0; i < courses; i++) {
        m_signalMapper->setMapping(m_courses.at(i), i);
    }
}

QVariant CourseModel::course(int row) const
{
    return data(index(row, 0), CourseModel::DataRole);
}
