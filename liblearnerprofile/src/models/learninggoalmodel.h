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

#ifndef LEARNINGGOALMODEL_H
#define LEARNINGGOALMODEL_H

#include "../liblearnerprofile_export.h"
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
    enum learningGoalRoles {
        TitleRole = Qt::UserRole + 1,
        IdRole,
        DataRole
    };

    explicit LearningGoalModel(QObject *parent = 0);
    virtual ~LearningGoalModel();
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int,QByteArray> roleNames() const Q_DECL_OVERRIDE;
    void setProfileManager(ProfileManager *profileManager);
    ProfileManager * profileManager() const;
    void setLearner(Learner *learner);
    Learner * learner() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
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
