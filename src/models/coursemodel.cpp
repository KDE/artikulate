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
#include "application.h"
#include "artikulate_debug.h"
#include "core/ilanguage.h"
#include "core/iresourcerepository.h"
#include "core/icourse.h"
#include <QAbstractListModel>
#include <KLocalizedString>

CourseModel::CourseModel(QObject *parent)
    : CourseModel(artikulateApp->resourceRepository(), parent)
{
}

CourseModel::CourseModel(IResourceRepository *repository, QObject *parent)
    : QAbstractListModel(parent)
{
    setResourceRepository(repository);
}

QHash< int, QByteArray > CourseModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[I18nTitleRole] = "i18nTitle";
    roles[DescriptionRole] = "description";
    roles[IdRole] = "id";
    roles[LanguageRole] = "language";
    roles[DataRole] = "dataRole";

    return roles;
}

void CourseModel::setResourceRepository(IResourceRepository *resourceRepository)
{
    for (auto &connection : m_updateConnections) {
        QObject::disconnect(connection);
    }
    m_updateConnections.clear();

    if (resourceRepository == nullptr) {
        qCWarning(ARTIKULATE_CORE()) << "Setting resource repository to nullptr, this shall never happen";
    }
    Q_ASSERT(resourceRepository != nullptr);

    if (m_resourceRepository == resourceRepository) {
        qCWarning(ARTIKULATE_CORE()) << "Skipping repository setting, it does not change";
        return;
    }

    beginResetModel();

    if (m_resourceRepository) {
        disconnect(m_resourceRepository, &IResourceRepository::courseAboutToBeAdded, this, &CourseModel::onCourseAboutToBeAdded);
        disconnect(m_resourceRepository, &IResourceRepository::courseAdded, this, &CourseModel::onCourseAdded);
        disconnect(m_resourceRepository, &IResourceRepository::courseAboutToBeRemoved, this, &CourseModel::onCourseAboutToBeRemoved);
    }
    m_resourceRepository = resourceRepository;
    if (m_resourceRepository) {
        connect(m_resourceRepository, &IResourceRepository::courseAboutToBeAdded, this, &CourseModel::onCourseAboutToBeAdded);
        connect(m_resourceRepository, &IResourceRepository::courseAdded, this, &CourseModel::onCourseAdded);
        connect(m_resourceRepository, &IResourceRepository::courseAboutToBeRemoved, this, &CourseModel::onCourseAboutToBeRemoved);
    }
    if (m_resourceRepository) {
        auto courses = m_resourceRepository->courses();
        for (int i = 0; i < courses.count(); ++i) {
            auto course = courses.at(i);
            // TODO only title changed is connected, change this to a general changed signal
            auto connection = connect(course.get(), &ICourse::titleChanged, this, [=](){
                const auto row = m_resourceRepository->courses().indexOf(course);
                emit dataChanged(index(row, 0), index(row, 0));
            });
            m_updateConnections.insert(i, connection);
        }
    }
    endResetModel();
}

IResourceRepository * CourseModel::resourceRepository() const
{
    return m_resourceRepository;
}

QVariant CourseModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_resourceRepository) {
        return QVariant();
    }
    if (index.row() >= rowCount()) {
        return QVariant();
    }

    auto const course = m_resourceRepository->courses().at(index.row());

    switch(role)
    {
    case Qt::DisplayRole:
        return !course->title().isEmpty()?
                QVariant(course->title()): QVariant(i18nc("@item:inlistbox:", "unknown"));
    case Qt::ToolTipRole:
        return QVariant(course->title());
    case TitleRole:
        return course->title();
    case I18nTitleRole:
        return course->i18nTitle();
    case DescriptionRole:
        return course->description();
    case IdRole:
        return course->id();
    case LanguageRole:
        return QVariant::fromValue<QObject*>(course->language().get());
    case DataRole:
        return QVariant::fromValue<QObject*>(course.get());
    default:
        return QVariant();
    }
}

int CourseModel::rowCount(const QModelIndex&) const
{
    if (!m_resourceRepository) {
        return 0;
    }
    return m_resourceRepository->courses().count();
}

void CourseModel::onCourseAboutToBeAdded(std::shared_ptr<ICourse> course, int row)
{
    beginInsertRows(QModelIndex(), row, row);
    auto connection = connect(course.get(), &ICourse::titleChanged, this, [=](){
        const auto row = m_resourceRepository->courses().indexOf(course);
        emit dataChanged(index(row, 0), index(row, 0));
    });
    m_updateConnections.insert(row, connection);
}

void CourseModel::onCourseAdded()
{
    endInsertRows();
}

void CourseModel::onCourseAboutToBeRemoved(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    QObject::disconnect(m_updateConnections.at(row));
    m_updateConnections.removeAt(row);
    endRemoveRows();
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

QVariant CourseModel::course(int row) const
{
    return data(index(row, 0), CourseModel::DataRole);
}
