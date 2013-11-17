/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) version 3, or any
 *  later version accepted by the membership of KDE e.V. (or its
 *  successor approved by the membership of KDE e.V.), which shall
 *  act as a proxy defined in Section 6 of version 3 of the license.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <QObject>

class QSqlError;
class QSqlDatabase;

namespace LearnerProfile
{

class Learner;
class LearningGoal;

class Storage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    explicit Storage(QObject* parent = 0);
    QString errorMessage() const;

    /**
     * Store profile in database. This can either be a new or an existing profile.
     * If it is an existing profile, the corresponding values are updated.
     */
    bool storeProfile(Learner *learner);
    bool removeProfile(Learner *learner);
    QList<Learner *> loadProfiles();
    bool storeGoal(LearningGoal *goal);
    QList<LearningGoal *> loadGoals();

Q_SIGNALS:
    void errorMessageChanged();

protected:
    QSqlDatabase database();
    void raiseError(const QSqlError &error);

private:
    bool updateSchema();
    QString m_errorMessage;
};
}

#endif // STORAGE_H
