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
#include "core/course.h"
#include "core/resourcemanager.h"
#include "core/skeleton.h"
#include "core/resources/skeletonresource.h"

#include <QAbstractListModel>
#include <QSignalMapper>
#include <KLocalizedString>
#include "artikulate_debug.h"

SkeletonModel::SkeletonModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_resourceManager(nullptr)
    , m_signalMapper(new QSignalMapper(this))
{
    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitSkeletonChanged(int)));
}

QHash< int, QByteArray > SkeletonModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[DescriptionRole] = "description";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";

    return roles;
}

void SkeletonModel::setResourceManager(ResourceManager *resourceManager)
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
        connect(m_resourceManager, &ResourceManager::skeletonAboutToBeAdded, this, &SkeletonModel::onSkeletonAboutToBeAdded);
        connect(m_resourceManager, &ResourceManager::skeletonAdded, this, &SkeletonModel::onSkeletonAdded);
        connect(m_resourceManager, &ResourceManager::skeletonAboutToBeRemoved, this, &SkeletonModel::onSkeletonsAboutToBeRemoved);
        connect(m_resourceManager, &ResourceManager::skeletonRemoved, this, &SkeletonModel::onSkeletonsRemoved);
    }

    endResetModel();

    emit resourceManagerChanged();
}

ResourceManager * SkeletonModel::resourceManager() const
{
    return m_resourceManager;
}

QVariant SkeletonModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_resourceManager->skeletonResources().count()) {
        return QVariant();
    }

    Skeleton * const skeleton = m_resourceManager->skeletonResources().at(index.row())->skeleton();

    switch(role)
    {
    case Qt::DisplayRole:
        return !skeleton->title().isEmpty() ?
                QVariant(skeleton->title()): QVariant(i18nc("@item:inlistbox:", "unknown"));
    case Qt::ToolTipRole:
        return QVariant(skeleton->title());
    case TitleRole:
        return skeleton->title();
    case DescriptionRole:
        return skeleton->description();
    case IdRole:
        return skeleton->id();
    case DataRole:
        return QVariant::fromValue<QObject*>(skeleton);
    default:
        return QVariant();
    }
}

int SkeletonModel::rowCount(const QModelIndex &parent) const
{
    if (!m_resourceManager) {
        return 0;
    }

    if (parent.isValid()) {
        return 0;
    }

    return m_resourceManager->skeletonResources().count();
}

void SkeletonModel::onSkeletonAboutToBeAdded(Course *skeleton, int index)
{
    connect(skeleton, SIGNAL(titleChanged()), m_signalMapper, SLOT(map()));
    //TODO add missing signals
    beginInsertRows(QModelIndex(), index, index);
}

void SkeletonModel::onSkeletonAdded()
{
    updateMappings();
    endInsertRows();
    emit countChanged();
}

void SkeletonModel::onSkeletonsAboutToBeRemoved(int first, int last)
{
    beginRemoveRows(QModelIndex(), first, last);
}

void SkeletonModel::onSkeletonsRemoved()
{
    endRemoveRows();
    emit countChanged();
}

void SkeletonModel::emitSkeletonChanged(int row)
{
    emit skeletonChanged(row);
    emit dataChanged(index(row, 0), index(row, 0));
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

int SkeletonModel::count() const
{
    return m_resourceManager->skeletonResources().count();
}

void SkeletonModel::updateMappings()
{
    int skeletons = m_resourceManager->skeletonResources().count();
    for (int i = 0; i < skeletons; ++i) {
        m_signalMapper->setMapping(m_resourceManager->skeletonResources().at(i)->skeleton(), i);
    }
}

QVariant SkeletonModel::skeleton(int row) const
{
    return data(index(row, 0), SkeletonModel::DataRole);
}
