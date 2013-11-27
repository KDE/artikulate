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

#include "learninggoalmodel.h"
#include "profilemanager.h"
#include "learner.h"

#include <QAbstractListModel>
#include <QSignalMapper>

#include <KLocale>
#include <KDebug>

using namespace LearnerProfile;

// private class LearningGoalModelPrivate
class LearningGoalModelPrivate {
public:
    LearningGoalModelPrivate()
        : m_profileManager(0)
        , m_learner(0)
    {
    }

    ~LearningGoalModelPrivate() {}
    void updateGoals();

    ProfileManager *m_profileManager;
    Learner *m_learner;
    QList<LearningGoal*> m_goals;
};

void LearningGoalModelPrivate::updateGoals()
{
    m_goals.clear();
    // if learner is set, only set its goals
    if (m_learner) {
        foreach (LearningGoal *goal, m_learner->goals()) {
            m_goals.append(goal);
        }
        return;
    }
    // else set all registered goals from profile manager
    if (m_profileManager) {
        foreach (LearningGoal *goal, m_profileManager->goals()) {
            m_goals.append(goal);
        }
    }
}

// class LearningGoalModel
LearningGoalModel::LearningGoalModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new LearningGoalModelPrivate)
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";
    setRoleNames(roles);
}

LearningGoalModel::~LearningGoalModel()
{

}

void LearningGoalModel::setProfileManager(ProfileManager *profileManager)
{
    if (d->m_profileManager == profileManager) {
        return;
    }

    beginResetModel();

    if (d->m_profileManager) {
        d->m_profileManager->disconnect(this);
    }

    d->m_profileManager = profileManager;
    d->updateGoals();
    endResetModel();

    emit profileManagerChanged();
}

ProfileManager * LearningGoalModel::profileManager() const
{
    return d->m_profileManager;
}

Learner * LearningGoalModel::learner() const
{
    return d->m_learner;
}

void LearningGoalModel::setLearner(Learner *learner)
{
    if (!learner) {
        return;
    }
    emit beginResetModel();
    d->m_learner = learner;
    d->updateGoals();
    emit learnerChanged();
    emit endResetModel();
}

QVariant LearningGoalModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= d->m_goals.count()) {
        return QVariant();
    }

    LearningGoal * const goal = d->m_goals.at(index.row());

    switch(role)
    {
    case Qt::DisplayRole:
        return !goal->name().isEmpty()?
                QVariant(goal->name()): QVariant(i18nc("@item:inlistbox:", "unknown"));
    case Qt::ToolTipRole:
        return QVariant(goal->name());
    case TitleRole:
        return goal->name();
    case IdRole:
        return goal->identifier();
    case DataRole:
        return QVariant::fromValue<QObject*>(goal);
    default:
        return QVariant();
    }
}

int LearningGoalModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return d->m_goals.count();
}

void LearningGoalModel::onLearningGoalAboutToBeAdded(LearningGoal *goal, int index)
{
    Q_UNUSED(index)
    beginInsertRows(QModelIndex(), d->m_goals.count(), d->m_goals.count());
    d->m_goals.append(goal);
}

void LearningGoalModel::onLearningGoalAdded()
{
    endInsertRows();
}

void LearningGoalModel::onLearningGoalAboutToBeRemoved(int index)
{
    if (!d->m_learner) {
        return;
    }

    if (index < 0 || d->m_goals.count() <= index) {
        kWarning() << "Cannot remove learning goal from model, not registered";
        return;
    }
    beginRemoveRows(QModelIndex(), index, index);
    d->m_goals.removeAt(index);
    endRemoveRows();
}

QVariant LearningGoalModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }
    if (orientation == Qt::Vertical) {
        return QVariant(section + 1);
    }
    return QVariant(i18nc("@title:column", "Learning Goal"));
}

QVariant LearningGoalModel::learningGoal(int row) const
{
    return data(index(row, 0), LearningGoalModel::DataRole);
}
