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

#include "skeletonmodel.h"
#include "application.h"
#include "core/contributorrepository.h"
#include "core/icourse.h"
#include "core/resources/skeletonresource.h"

#include "artikulate_debug.h"
#include <KLocalizedString>
#include <QAbstractListModel>
#include <QSignalMapper>

SkeletonModel::SkeletonModel(QObject *parent)
    : SkeletonModel(artikulateApp->editableRepository(), parent)
{
}

SkeletonModel::SkeletonModel(IEditableRepository *repository, QObject *parent)
    : QAbstractListModel(parent)
{
    setResourceRepository(repository);
}

QHash<int, QByteArray> SkeletonModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[DescriptionRole] = "description";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";
    return roles;
}

void SkeletonModel::setResourceRepository(IEditableRepository *repository)
{
    if (m_repository == repository) {
        return;
    }

    beginResetModel();

    if (m_repository) {
        disconnect(m_repository, &IEditableRepository::skeletonAboutToBeAdded, this, &SkeletonModel::onSkeletonAboutToBeAdded);
        disconnect(m_repository, &IEditableRepository::skeletonAdded, this, &SkeletonModel::onSkeletonAdded);
        disconnect(m_repository, &IEditableRepository::skeletonAboutToBeRemoved, this, &SkeletonModel::onSkeletonAboutToBeRemoved);
        disconnect(m_repository, &IEditableRepository::skeletonRemoved, this, &SkeletonModel::onSkeletonRemoved);
    }

    m_repository = repository;

    if (m_repository) {
        connect(m_repository, &IEditableRepository::skeletonAboutToBeAdded, this, &SkeletonModel::onSkeletonAboutToBeAdded);
        connect(m_repository, &IEditableRepository::skeletonAdded, this, &SkeletonModel::onSkeletonAdded);
        connect(m_repository, &IEditableRepository::skeletonAboutToBeRemoved, this, &SkeletonModel::onSkeletonAboutToBeRemoved);
        connect(m_repository, &IEditableRepository::skeletonRemoved, this, &SkeletonModel::onSkeletonRemoved);
    }

    if (m_repository) {
        auto skeletons = m_repository->skeletons();
        for (int i = 0; i < skeletons.count(); ++i) {
            auto skeleton = skeletons.at(i);
            // TODO only title changed is connected, change this to a general changed signal
            auto connection = connect(skeleton.get(), &IEditableCourse::titleChanged, this, [=]() {
                const auto row = m_repository->skeletons().indexOf(skeleton);
                emit dataChanged(index(row, 0), index(row, 0));
            });
            m_updateConnections.insert(i, connection);
        }
    }

    endResetModel();
}

IEditableRepository *SkeletonModel::resourceRepository() const
{
    return m_repository;
}

QVariant SkeletonModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_repository == nullptr) {
        return QVariant();
    }

    if (index.row() >= rowCount()) {
        return QVariant();
    }

    auto skeleton = m_repository->skeletons().at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return !skeleton->title().isEmpty() ? QVariant(skeleton->title()) : QVariant(i18nc("@item:inlistbox:", "unknown"));
        case Qt::ToolTipRole:
            return QVariant(skeleton->title());
        case TitleRole:
            return skeleton->title();
        case DescriptionRole:
            return skeleton->description();
        case IdRole:
            return skeleton->id();
        case DataRole:
            return QVariant::fromValue<QObject *>(skeleton.get());
        default:
            return QVariant();
    }
}

int SkeletonModel::rowCount(const QModelIndex &) const
{
    if (m_repository == nullptr) {
        return 0;
    }
    return m_repository->skeletons().count();
}

void SkeletonModel::onSkeletonAboutToBeAdded(std::shared_ptr<IEditableCourse> skeleton, int row)
{
    beginInsertRows(QModelIndex(), row, row);
    auto connection = connect(skeleton.get(), &IEditableCourse::titleChanged, this, [=]() {
        const auto row = m_repository->courses().indexOf(skeleton);
        emit dataChanged(index(row, 0), index(row, 0));
    });
    m_updateConnections.insert(row, connection);
}

void SkeletonModel::onSkeletonAdded()
{
    endInsertRows();
}

void SkeletonModel::onSkeletonAboutToBeRemoved(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    QObject::disconnect(m_updateConnections.at(row));
    m_updateConnections.removeAt(row);
}

void SkeletonModel::onSkeletonRemoved()
{
    endRemoveRows();
}

QVariant SkeletonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    return QVariant(i18nc("@title:column", "Skeleton"));
}

QVariant SkeletonModel::skeleton(int row) const
{
    return data(index(row, 0), SkeletonModel::DataRole);
}
