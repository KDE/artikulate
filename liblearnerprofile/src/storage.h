/*
 *  Copyright 2013-2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
    explicit Storage(QObject *parent = nullptr);
    /**
     * \note this constructor is tailored for unit tests
     */
    explicit Storage(const QString &databasePath, QObject *parent = nullptr);
    QString errorMessage() const;

    /**
     * Store profile in database. This can either be a new or an existing profile.
     * If it is an existing profile, the corresponding values are updated.
     */
    bool storeProfile(Learner *learner);
    bool removeProfile(Learner *learner);
    bool removeRelation(Learner *learner, LearningGoal *goal);
    QList<Learner *> loadProfiles(QList<LearnerProfile::LearningGoal *> goals);
    bool storeGoal(LearningGoal *goal);
    QList<LearningGoal *> loadGoals();
    bool storeProgressLog(Learner *learner, LearningGoal *goal, const QString &container, const QString &item, int payload, const QDateTime &time);
    /**
     * Load list of progress values for specified item
     * \return list of date/payload values for this item
     */
    QList<QPair<QDateTime, int>> readProgressLog(Learner *learner, LearningGoal *goal, const QString &container, const QString &item);
    bool storeProgressValue(Learner *learner, LearningGoal *goal, const QString &container, const QString &item, int payload);
    /**
     * Load list of progress values for specified container
     * \return list of item/payload values for all items in container
     */
    QHash<QString, int> readProgressValues(Learner *learner, LearningGoal *goal, const QString &container);
    /**
     * Load payload value of specified item. If no value is found, \return -1
     */
    int readProgressValue(Learner *learner, LearningGoal *goal, const QString &container, const QString &item);

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
