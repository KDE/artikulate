/*
    SPDX-FileCopyrightText: 2013-2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef LEARNINGGOALMODEL_H
#define LEARNINGGOALMODEL_H

#include "liblearnerprofile_export.h"
#include <QAbstractListModel>
#include <QObject>

class LearningGoalModelPrivate;

namespace LearnerProfile
{
class ProfileManager;
class LearningGoal;
class Learner;

class LIBLEARNERPROFILE_EXPORT LearningGoalModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(LearnerProfile::ProfileManager *profileManager READ profileManager WRITE setProfileManager NOTIFY profileManagerChanged)
    Q_PROPERTY(LearnerProfile::Learner *learner READ learner WRITE setLearner NOTIFY learnerChanged)

public:
    enum learningGoalRoles { TitleRole = Qt::UserRole + 1, IdRole, DataRole };

    explicit LearningGoalModel(QObject *parent = nullptr);
    virtual ~LearningGoalModel();
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int, QByteArray> roleNames() const override;
    void setProfileManager(ProfileManager *profileManager);
    ProfileManager *profileManager() const;
    void setLearner(Learner *learner);
    Learner *learner() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE QVariant learningGoal(int index) const;

Q_SIGNALS:
    void profileManagerChanged();
    void learnerChanged();
    void learningGoalChanged(int index);

private Q_SLOTS:
    void onLearningGoalAboutToBeAdded(LearningGoal *goal, int index);
    void onLearningGoalAdded();
    void onLearningGoalAboutToBeRemoved(int index);
    void emitLearningGoalChanged(int row);

private:
    const QScopedPointer<LearningGoalModelPrivate> d;
};
}

#endif // LEARNINGGOALMODEL_H
