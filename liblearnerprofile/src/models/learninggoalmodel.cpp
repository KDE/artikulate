/*
    SPDX-FileCopyrightText: 2013-2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "learninggoalmodel.h"
#include "learner.h"
#include "profilemanager.h"

#include <QSignalMapper>

#include "liblearner_debug.h"
#include <KLocalizedString>

using namespace LearnerProfile;

// private class LearningGoalModelPrivate
class LearningGoalModelPrivate
{
public:
    LearningGoalModelPrivate()
        : m_profileManager(nullptr)
        , m_learner(nullptr)
        , m_signalMapper(new QSignalMapper())
    {
    }

    ~LearningGoalModelPrivate()
    {
        delete m_signalMapper;
    }

    void updateGoals();
    void updateMappings();

    ProfileManager *m_profileManager;
    Learner *m_learner;
    QList<LearningGoal *> m_goals;
    QSignalMapper *m_signalMapper;
};

void LearningGoalModelPrivate::updateGoals()
{
    m_goals.clear();
    // set all registered goals from profile manager
    if (m_profileManager) {
        for (LearningGoal *goal : m_profileManager->goals()) {
            m_goals.append(goal);
        }
    }
    // TODO add learner status information
}

void LearningGoalModelPrivate::updateMappings()
{
    if (!m_profileManager) {
        return;
    }
    int goals = m_goals.count();
    for (int i = 0; i < goals; ++i) {
        m_signalMapper->setMapping(m_goals.at(i), i);
    }
}

// class LearningGoalModel
LearningGoalModel::LearningGoalModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new LearningGoalModelPrivate)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    connect(d->m_signalMapper, static_cast<void (QSignalMapper::*)(int)>(&QSignalMapper::mapped), this, &LearningGoalModel::Q_EMITLearningGoalChanged);
#else
    connect(d->m_signalMapper, &QSignalMapper::mappedInt, this, &LearningGoalModel::emitLearningGoalChanged);
#endif
}

LearningGoalModel::~LearningGoalModel()
{
}

QHash<int, QByteArray> LearningGoalModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[IdRole] = "id";
    roles[DataRole] = "dataRole";

    return roles;
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
    d->updateMappings();
    endResetModel();

    Q_EMIT profileManagerChanged();
}

ProfileManager *LearningGoalModel::profileManager() const
{
    return d->m_profileManager;
}

Learner *LearningGoalModel::learner() const
{
    return d->m_learner;
}

void LearningGoalModel::setLearner(Learner *learner)
{
    if (!learner) {
        return;
    }
    beginResetModel();
    if (d->m_learner) {
        learner->disconnect(this);
    }
    d->m_learner = learner;
    d->updateGoals();
    d->updateMappings();
    connect(learner, &Learner::goalAboutToBeAdded, this, &LearningGoalModel::onLearningGoalAboutToBeAdded);
    connect(learner, &Learner::goalAdded, this, &LearningGoalModel::onLearningGoalAdded);
    connect(learner, &Learner::goalAboutToBeRemoved, this, &LearningGoalModel::onLearningGoalAboutToBeRemoved);
    Q_EMIT learnerChanged();
    endResetModel();
}

QVariant LearningGoalModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() >= d->m_goals.count()) {
        return QVariant();
    }

    LearningGoal *const goal = d->m_goals.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return !goal->name().isEmpty() ? QVariant(goal->name()) : QVariant(i18nc("@item:inlistbox unknown learning goal", "unknown"));
    case Qt::ToolTipRole:
        return QVariant(goal->name());
    case TitleRole:
        return goal->name();
    case IdRole:
        return goal->identifier();
    case DataRole:
        return QVariant::fromValue<QObject *>(goal);
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
    d->updateMappings();
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
        qCWarning(LIBLEARNER_LOG) << "Cannot remove learning goal from model, not registered";
        return;
    }
    beginRemoveRows(QModelIndex(), index, index);
    d->m_goals.removeAt(index);
    d->updateMappings();
    endRemoveRows();
}

void LearningGoalModel::emitLearningGoalChanged(int row)
{
    Q_EMIT learningGoalChanged(row);
    Q_EMIT dataChanged(index(row, 0), index(row, 0));
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
