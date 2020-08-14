/*
 *  SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "profilemodel.h"
#include "liblearnerprofile/src/learner.h"
#include "liblearnerprofile/src/profilemanager.h"

#include "artikulate_debug.h"
#include <KLocalizedString>
#include <QAbstractListModel>
#include <QSignalMapper>

using namespace LearnerProfile;

ProfileModel::ProfileModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_profileManager(nullptr)
    , m_signalMapper(new QSignalMapper(this))
{
    connect(m_signalMapper, SIGNAL(mapped(int)), SLOT(emitProfileChanged(int)));
}

QHash<int, QByteArray> ProfileModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[NameRole] = "name";
    roles[DataRole] = "dataRole";

    return roles;
}

void ProfileModel::setProfileManager(ProfileManager *profileManager)
{
    if (m_profileManager == profileManager) {
        return;
    }

    beginResetModel();

    if (m_profileManager) {
        m_profileManager->disconnect(this);
        foreach (Learner *learner, m_profileManager->profiles()) {
            learner->disconnect(this);
        }
    }

    m_profileManager = profileManager;
    if (m_profileManager) {
        // initial setting of signal mappings
        connect(m_profileManager, &ProfileManager::profileAdded, this, &ProfileModel::onProfileAdded);
        connect(m_profileManager, &ProfileManager::profileAboutToBeRemoved, this, &ProfileModel::onProfileAboutToBeRemoved);

        // insert and connect all already existing profiles
        int profiles = m_profileManager->profiles().count();
        for (int i = 0; i < profiles; ++i) {
            onProfileAdded(m_profileManager->profiles().at(i), i);
        }
        updateMappings();
    }
    endResetModel();
}

ProfileManager *ProfileModel::profileManager() const
{
    return m_profileManager;
}

QVariant ProfileModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(m_profileManager);

    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= m_profileManager->profiles().count()) {
        return QVariant();
    }

    Learner *const learner = m_profileManager->profiles().at(index.row());

    switch (role) {
        case Qt::DisplayRole:
            return !learner->name().isEmpty() ? QVariant(learner->name()) : QVariant(i18nc("@item:inlistbox:", "unknown"));
        case Qt::ToolTipRole:
            return QVariant(learner->name());
        case IdRole:
            return learner->identifier();
        case NameRole:
            return learner->name();
        case DataRole:
            return QVariant::fromValue<QObject *>(learner);
        default:
            return QVariant();
    }
}

int ProfileModel::rowCount(const QModelIndex &parent) const
{
    if (!m_profileManager) {
        return 0;
    }

    if (parent.isValid()) {
        return 0;
    }
    return m_profileManager->profiles().count();
}

void ProfileModel::onProfileAdded(Learner *learner, int index)
{
    connect(learner, SIGNAL(nameChanged()), m_signalMapper, SLOT(map()));
    connect(learner, SIGNAL(identifierChanged()), m_signalMapper, SLOT(map()));
    beginInsertRows(QModelIndex(), index, index);
    updateMappings();
    endInsertRows();
}

void ProfileModel::onProfileAboutToBeRemoved(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    endRemoveRows();
}

void ProfileModel::emitProfileChanged(int row)
{
    beginResetModel();
    endResetModel();
    // FIXME very inefficient, but workaround to force new filtering in phrasefiltermodel
    //      to exclude possible new excluded phrases
    emit profileChanged(row);
    emit dataChanged(index(row, 0), index(row, 0));
}

QVariant ProfileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    return QVariant(i18nc("@title:column", "Profile"));
}

void ProfileModel::updateMappings()
{
    if (!m_profileManager) {
        return;
    }
    int profiles = m_profileManager->profiles().count();
    for (int i = 0; i < profiles; ++i) {
        m_signalMapper->setMapping(m_profileManager->profiles().at(i), i);
    }
}
