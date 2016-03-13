/*
 *  Copyright 2013-2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

/**
 * \class Storage
 * Database storage for learner information database.
 */
class Storage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString errorMessage READ errorMessage NOTIFY errorMessageChanged)

public:
    /**
     * Default constructor, which sets a default database path at
     * DataLocation + learnerdata.db
     */
    explicit Storage(QObject* parent = nullptr);
    /**
     * \note this constructor is tailored for unit tests
     */
    Storage(const QString databasePath, QObject* parent = nullptr);
    QString errorMessage() const;

    /**
     * Store profile in database. This can either be a new or an existing profile.
     * If it is an existing profile, the corresponding values are updated.
     */
    bool storeProfile(Learner *learner);
    bool removeProfile(Learner *learner);
    bool removeRelation(Learner *learner, LearningGoal *goal);
    QList<Learner *> loadProfiles(QList< LearnerProfile::LearningGoal* > goals);
    bool storeGoal(LearningGoal *goal);
    QList<LearningGoal *> loadGoals();
    bool storeProgressLog(Learner *learner, LearningGoal *goal,
                       const QString &container, const QString &item, int payload,
                       const QDateTime &time);
    /**
     * Load list of progress values for specified item
     * \return list of date/payload values for this item
     */
    QList<QPair<QDateTime,int>> readProgressLog(Learner *learner, LearningGoal *goal,
                       const QString &container, const QString &item);
    bool storeProgressValue(Learner *learner, LearningGoal *goal,
                       const QString &container, const QString &item, int payload);
    /**
     * Load list of progress values for specified container
     * \return list of item/payload values for all items in container
     */
    QList<QPair<QString, int>> readProgressValues(Learner *learner, LearningGoal *goal,
                       const QString &container);

Q_SIGNALS:
    void errorMessageChanged();

protected:
    QSqlDatabase database();
    void raiseError(const QSqlError &error);

private:
    bool updateSchema();
    const QString m_databasePath;
    QString m_errorMessage;
};
}

#endif // STORAGE_H
