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

#include "storage.h"
#include "learner.h"
#include "liblearner_debug.h"

#include <KLocalizedString>

#include <QDateTime>
#include <QDir>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStandardPaths>

using namespace LearnerProfile;

Storage::Storage(QObject* parent)
    : QObject(parent)
    , m_databasePath(QStandardPaths::writableLocation(
        QStandardPaths::DataLocation) + QLatin1Char('/') + "learnerdata.db")
    , m_errorMessage(QString())
{

}

Storage::Storage(const QString &databasePath, QObject* parent)
    : QObject(parent)
    , m_databasePath(databasePath)
    , m_errorMessage(QString())
{
    qCDebug(LIBLEARNER_LOG) << "Initialize with custom DB path:" << m_databasePath;
}

QString Storage::errorMessage() const
{
    return m_errorMessage;
}

void Storage::raiseError(const QSqlError &error)
{
    m_errorMessage = QStringLiteral("%1 : %2").arg(error.driverText(), error.databaseText());
    emit errorMessageChanged();
}

bool Storage::storeProfile(Learner *learner)
{
    QSqlDatabase db = database();

    // test whether ID is present
    QSqlQuery idExistsQuery(db);
    idExistsQuery.prepare(QStringLiteral("SELECT COUNT(*) FROM profiles WHERE id = :id"));
    idExistsQuery.bindValue(QStringLiteral(":id"), learner->identifier());
    idExistsQuery.exec();
    if (db.lastError().isValid()) {
        qCritical() << "ExistsQuery: " << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }
    // go to first result row that contains the count
    idExistsQuery.next();
    if (idExistsQuery.value(0).toInt() < 1) {
        // in case learner ID is not found in database
        QSqlQuery insertProfileQuery(db);
        insertProfileQuery.prepare(QStringLiteral("INSERT INTO profiles (id, name) VALUES (?, ?)"));
        insertProfileQuery.bindValue(0, learner->identifier());
        insertProfileQuery.bindValue(1, learner->name());
        insertProfileQuery.exec();

        if (insertProfileQuery.lastError().isValid()) {
            raiseError(insertProfileQuery.lastError());
            db.rollback();
            return false;
        }
    } else {
        // update name otherwise
        QSqlQuery updateProfileQuery(db);
        updateProfileQuery.prepare(QStringLiteral("UPDATE profiles SET name = :name WHERE id = :id"));
        updateProfileQuery.bindValue(QStringLiteral(":id"), learner->identifier());
        updateProfileQuery.bindValue(QStringLiteral(":name"), learner->name());
        updateProfileQuery.exec();
        if (updateProfileQuery.lastError().isValid()) {
            qCritical() << updateProfileQuery.lastError().text();
            raiseError(updateProfileQuery.lastError());
            db.rollback();
            return false;
        }
    }

    // store existing learning goal relations
    foreach (LearningGoal *goal, learner->goals()) {
        QSqlQuery relationExistsQuery(db);
        relationExistsQuery.prepare("SELECT COUNT(*) FROM learner_goals "
            "WHERE goal_category = :goalCategory "
            "AND goal_identifier = :goalIdentifier "
            "AND profile_id = :profileId "
            );
        relationExistsQuery.bindValue(QStringLiteral(":goalCategory"), goal->category());
        relationExistsQuery.bindValue(QStringLiteral(":goalIdentifier"), goal->identifier());
        relationExistsQuery.bindValue(QStringLiteral(":profileId"), learner->identifier());
        relationExistsQuery.exec();
        if (db.lastError().isValid()) {
            qCritical() << "ExistsQuery: " << db.lastError().text();
            raiseError(db.lastError());
            return false;
        }
        // go to first result row that contains the count
        relationExistsQuery.next();
        if (relationExistsQuery.value(0).toInt() < 1) {
            QSqlQuery insertProfileQuery(db);
            insertProfileQuery.prepare(QStringLiteral("INSERT INTO learner_goals (goal_category, goal_identifier, profile_id) VALUES (?, ?, ?)"));
            insertProfileQuery.bindValue(0, goal->category());
            insertProfileQuery.bindValue(1, goal->identifier());
            insertProfileQuery.bindValue(2, learner->identifier());
            insertProfileQuery.exec();
        }
    }
    // remove deleted relations
    QSqlQuery cleanupRelations(db);
    cleanupRelations.prepare(QStringLiteral("DELETE FROM learner_goals WHERE "));
    //TODO change creation of relations to same way as remove-relations: explicit connections

    return true;
}

bool Storage::removeProfile(Learner *learner)
{
    QSqlDatabase db = database();
    QSqlQuery removeProfileQuery(db);

    // delete learner
    removeProfileQuery.prepare(QStringLiteral("DELETE FROM profiles WHERE id = ?"));
    removeProfileQuery.bindValue(0, learner->identifier());
    removeProfileQuery.exec();

    if (removeProfileQuery.lastError().isValid()) {
        qCritical() << removeProfileQuery.lastError().text();
        raiseError(removeProfileQuery.lastError());
        db.rollback();
        return false;
    }

    // delete learning goal relations
    QSqlQuery removeGoalRelationQuery(db);
    removeGoalRelationQuery.prepare(QStringLiteral("DELETE FROM learner_goals WHERE profile_id = ?"));
    removeGoalRelationQuery.bindValue(0, learner->identifier());
    removeGoalRelationQuery.exec();

    if (removeGoalRelationQuery.lastError().isValid()) {
        qCritical() << removeGoalRelationQuery.lastError().text();
        raiseError(removeGoalRelationQuery.lastError());
        db.rollback();
        return false;
    }

    return true;
}

bool Storage::removeRelation(Learner *learner, LearningGoal *goal)
{
    QSqlDatabase db = database();
    QSqlQuery removeGoalRelationQuery(db);
    removeGoalRelationQuery.prepare(
        "DELETE FROM learner_goals "
        "WHERE goal_category = :goalCategory "
        "AND goal_identifier = :goalIdentifier "
        "AND profile_id = :profileId "
    );
    removeGoalRelationQuery.bindValue(QStringLiteral(":goalCategory"), goal->category());
    removeGoalRelationQuery.bindValue(QStringLiteral(":goalIdentifier"), goal->identifier());
    removeGoalRelationQuery.bindValue(QStringLiteral(":profileId"), learner->identifier());
    removeGoalRelationQuery.exec();
    if (db.lastError().isValid()) {
        qCritical() << "ExistsQuery: " << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }

    return true;
}

QList< Learner* > Storage::loadProfiles(QList<LearningGoal*> goals)
{
    QSqlDatabase db = database();
    QSqlQuery profileQuery(db);
    profileQuery.prepare(QStringLiteral("SELECT id, name FROM profiles"));
    profileQuery.exec();
    if (profileQuery.lastError().isValid()) {
        qCritical() << profileQuery.lastError().text();
        raiseError(profileQuery.lastError());
        return QList<Learner*>();
    }
    QList<Learner*> profiles;
    while (profileQuery.next()) {
        Learner* profile = new Learner();
        profile->setIdentifier(profileQuery.value(0).toInt());
        profile->setName(profileQuery.value(1).toString());
        profiles.append(profile);
    }

    // associate to goals
    QSqlQuery goalRelationQuery(db);
    goalRelationQuery.prepare(QStringLiteral("SELECT goal_category, goal_identifier, profile_id FROM learner_goals"));
    goalRelationQuery.exec();
    if (goalRelationQuery.lastError().isValid()) {
        qCritical() << goalRelationQuery.lastError().text();
        raiseError(goalRelationQuery.lastError());
        return QList<Learner*>();
    }
    while (goalRelationQuery.next()) {
        Learner *learner = nullptr;
        LearningGoal *goal = nullptr;

        foreach (Learner *cmpProfile, profiles) {
            if (cmpProfile->identifier() == goalRelationQuery.value(2).toInt()) {
                learner = cmpProfile;
                break;
            }
        }
        if (!learner) {
            qCCritical(LIBLEARNER_LOG) << "Could not retrieve learner from database.";
            return QList<Learner*>();
        }
        foreach (LearningGoal *cmpGoal, goals) {
            if (cmpGoal->category() == goalRelationQuery.value(0).toInt()
                && cmpGoal->identifier() == goalRelationQuery.value(1).toString())
            {
                goal = cmpGoal;
                break;
            }
        }

        if (learner->goals().contains(goal)) {
            continue;
        }
        if (goal) {
            learner->addGoal(goal);
        }
    }

    return profiles;
}

bool Storage::storeGoal(LearningGoal *goal)
{
    QSqlDatabase db = database();

    // test whether ID is present
    QSqlQuery goalExistsQuery(db);
    goalExistsQuery.prepare(QStringLiteral("SELECT COUNT(*) FROM goals WHERE category = :category AND identifier = :identifier"));
    goalExistsQuery.bindValue(QStringLiteral(":identifier"), goal->identifier());
    goalExistsQuery.bindValue(QStringLiteral(":category"), static_cast<int>(goal->category()));
    goalExistsQuery.exec();
    if (db.lastError().isValid()) {
        qCritical() << "ExistsQuery: " << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }
    // go to first result row that contains the count
    goalExistsQuery.next();
    if (goalExistsQuery.value(0).toInt() < 1) {
        // in case learner ID is not found in database
        QSqlQuery insertGoalQuery(db);
        insertGoalQuery.prepare(QStringLiteral("INSERT INTO goals (category, identifier, name) VALUES (?, ?, ?)"));
        insertGoalQuery.bindValue(0, static_cast<int>(goal->category()));
        insertGoalQuery.bindValue(1, goal->identifier());
        insertGoalQuery.bindValue(2, goal->name());
        insertGoalQuery.exec();
        if (insertGoalQuery.lastError().isValid()) {
            raiseError(insertGoalQuery.lastError());
            db.rollback();
            return false;
        }
        return true;
    } else {
        // update name otherwise
        QSqlQuery updateGoalQuery(db);
        updateGoalQuery.prepare(QStringLiteral("UPDATE goals SET name = :name WHERE category = :category AND identifier = :identifier"));
        updateGoalQuery.bindValue(QStringLiteral(":category"), static_cast<int>(goal->category()));
        updateGoalQuery.bindValue(QStringLiteral(":identifier"), goal->identifier());
        updateGoalQuery.bindValue(QStringLiteral(":name"), goal->name());
        updateGoalQuery.exec();
        if (updateGoalQuery.lastError().isValid()) {
            qCritical() << updateGoalQuery.lastError().text();
            raiseError(updateGoalQuery.lastError());
            db.rollback();
            return false;
        }
        return true;
    }
}

QList< LearningGoal* > Storage::loadGoals()
{
    QSqlDatabase db = database();
    QSqlQuery goalQuery(db);
    goalQuery.prepare(QStringLiteral("SELECT category, identifier, name FROM goals"));
    goalQuery.exec();
    if (goalQuery.lastError().isValid()) {
        qCritical() << goalQuery.lastError().text();
        raiseError(goalQuery.lastError());
        return QList<LearningGoal*>();
    }

    QList<LearningGoal*> goals;
    while (goalQuery.next()) {
        LearningGoal::Category category = static_cast<LearningGoal::Category>(goalQuery.value(0).toInt());
        QString identifier = goalQuery.value(1).toString();
        QString name = goalQuery.value(2).toString();
        LearningGoal* goal = new LearningGoal(category, identifier);
        goal->setName(name);
        goals.append(goal);
    }
    return goals;
}

bool Storage::storeProgressLog(Learner *learner, LearningGoal *goal,
                            const QString &container, const QString &item, int payload,
                            const QDateTime &time)
{
    QSqlDatabase db = database();
    QSqlQuery insertQuery(db);
    insertQuery.prepare("INSERT INTO learner_progress_log "
        "(goal_category, goal_identifier, profile_id, item_container, item, payload, date) "
        "VALUES (:gcategory, :gidentifier, :pid, :container, :item, :payload, :date)");
    insertQuery.bindValue(QStringLiteral(":gcategory"), static_cast<int>(goal->category()));
    insertQuery.bindValue(QStringLiteral(":gidentifier"), goal->identifier());
    insertQuery.bindValue(QStringLiteral(":pid"), learner->identifier());
    insertQuery.bindValue(QStringLiteral(":container"), container);
    insertQuery.bindValue(QStringLiteral(":item"), item);
    insertQuery.bindValue(QStringLiteral(":payload"), payload);
    insertQuery.bindValue(QStringLiteral(":date"), time.toString(Qt::ISODate));
    insertQuery.exec();

    if (insertQuery.lastError().isValid()) {
        raiseError(insertQuery.lastError());
        qCCritical(LIBLEARNER_LOG) << "DB Error:" << m_errorMessage;
        db.rollback();
        return false;
    }
    return true;
}

QList<QPair<QDateTime,int>> Storage::readProgressLog(Learner *learner, LearningGoal *goal,
                            const QString &container, const QString &item)
{
    QSqlDatabase db = database();
    QSqlQuery logQuery(db);
    logQuery.prepare("SELECT date, payload FROM learner_progress_log "
        "WHERE goal_category = :goalcategory "
        "AND goal_identifier = :goalid "
        "AND profile_id = :profileid "
        "AND item_container = :container "
        "AND item = :item");
    logQuery.bindValue(QStringLiteral(":goalcategory"), static_cast<int>(goal->category()));
    logQuery.bindValue(QStringLiteral(":goalid"), goal->identifier());
    logQuery.bindValue(QStringLiteral(":profileid"), learner->identifier());
    logQuery.bindValue(QStringLiteral(":container"), container);
    logQuery.bindValue(QStringLiteral(":item"), item);
    logQuery.exec();
    if (logQuery.lastError().isValid()) {
        qCritical() << logQuery.lastError().text();
        raiseError(logQuery.lastError());
        return QList<QPair<QDateTime,int>>();
    }

    QList<QPair<QDateTime,int>> log;
    while (logQuery.next()) {
        const QDateTime date{logQuery.value(0).toDateTime()};
        int payload{logQuery.value(1).toInt()};
        log.append(qMakePair(date, payload));
    }
    return log;
}

bool Storage::storeProgressValue(Learner *learner, LearningGoal *goal,
                            const QString &container, const QString &item, int payload)
{
    QSqlDatabase db = database();
    QSqlQuery query(db);

    // test if already payload stored
    query.prepare("SELECT payload FROM learner_progress_value "
        "WHERE goal_category = :gcategory "
        "AND goal_identifier = :gidentifier "
        "AND profile_id = :pid "
        "AND item_container = :container "
        "AND item = :item");
    query.bindValue(QStringLiteral(":gcategory"), static_cast<int>(goal->category()));
    query.bindValue(QStringLiteral(":gidentifier"), goal->identifier());
    query.bindValue(QStringLiteral(":pid"), learner->identifier());
    query.bindValue(QStringLiteral(":container"), container);
    query.bindValue(QStringLiteral(":item"), item);
    query.exec();
    if (query.lastError().isValid()) {
        qCritical() << query.lastError().text();
        raiseError(query.lastError());
        return false;
    }
    // if query contains values, perform update query
    if (query.next()) {
        query.finish(); // release resources from previous query
        query.prepare("UPDATE learner_progress_value "
            "SET payload = :payload "
            "WHERE goal_category = :gcategory "
            "AND goal_identifier = :gidentifier "
            "AND profile_id = :pid "
            "AND item_container = :container "
            "AND item = :item");
        query.bindValue(QStringLiteral(":payload"), static_cast<int>(payload));
        query.bindValue(QStringLiteral(":gcategory"), static_cast<int>(goal->category()));
        query.bindValue(QStringLiteral(":gidentifier"), goal->identifier());
        query.bindValue(QStringLiteral(":pid"), learner->identifier());
        query.bindValue(QStringLiteral(":container"), container);
        query.bindValue(QStringLiteral(":item"), item);
        query.exec();

        if (query.lastError().isValid()) {
            qCritical() << query.lastError().text();
            raiseError(query.lastError());
            db.rollback();
            return false;
        }
        return true;
    }
    // else insert new row
    else {
        query.finish(); // release resources from previous query
        query.prepare("INSERT INTO learner_progress_value "
            "(goal_category, goal_identifier, profile_id, item_container, item, payload) "
            "VALUES (:gcategory, :gidentifier, :pid, :container, :item, :payload)");
        query.bindValue(QStringLiteral(":gcategory"), static_cast<int>(goal->category()));
        query.bindValue(QStringLiteral(":gidentifier"), goal->identifier());
        query.bindValue(QStringLiteral(":pid"), learner->identifier());
        query.bindValue(QStringLiteral(":container"), container);
        query.bindValue(QStringLiteral(":item"), item);
        query.bindValue(QStringLiteral(":payload"), static_cast<int>(payload));
        query.exec();

        if (query.lastError().isValid()) {
            qCritical() << query.lastError().text();
            raiseError(query.lastError());
            db.rollback();
            return false;
        }
        return true;
    }
    Q_UNREACHABLE();
    return false;
}

QHash<QString,int> Storage::readProgressValues(Learner *learner, LearningGoal *goal,
                            const QString &container)
{
    QSqlDatabase db = database();
    QSqlQuery query(db);
    query.prepare("SELECT item, payload FROM learner_progress_value "
        "WHERE goal_category = :goalcategory "
        "AND goal_identifier = :goalid "
        "AND profile_id = :profileid "
        "AND item_container = :container");
    query.bindValue(QStringLiteral(":goalcategory"), static_cast<int>(goal->category()));
    query.bindValue(QStringLiteral(":goalid"), goal->identifier());
    query.bindValue(QStringLiteral(":profileid"), learner->identifier());
    query.bindValue(QStringLiteral(":container"), container);
    query.exec();
    if (query.lastError().isValid()) {
        qCritical() << query.lastError().text();
        raiseError(query.lastError());
        return QHash<QString,int>();
    }

    QHash<QString,int> values;
    while (query.next()) {
        const QString item{query.value(0).toString()};
        const int payload{query.value(1).toInt()};
        values.insert(item, payload);
    }
    return values;
}

int Storage::readProgressValue(Learner *learner, LearningGoal *goal,
                            const QString &container, const QString &item)
{
    QSqlDatabase db = database();
    QSqlQuery query(db);
    query.prepare("SELECT payload FROM learner_progress_value "
        "WHERE goal_category = :goalcategory "
        "AND goal_identifier = :goalid "
        "AND profile_id = :profileid "
        "AND item_container = :container "
        "AND item = :item");
    query.bindValue(QStringLiteral(":goalcategory"), static_cast<int>(goal->category()));
    query.bindValue(QStringLiteral(":goalid"), goal->identifier());
    query.bindValue(QStringLiteral(":profileid"), learner->identifier());
    query.bindValue(QStringLiteral(":container"), container);
    query.bindValue(QStringLiteral(":item"), item);
    query.exec();
    if (query.lastError().isValid()) {
        qCritical() << query.lastError().text();
        raiseError(query.lastError());
        return -1;
    }

    if (query.next()) {
        return query.value(0).toInt();
    }
    return -1;
}

QSqlDatabase Storage::database()
{
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        return QSqlDatabase::database(QSqlDatabase::defaultConnection);
    }

    // create data directory if it does not exist
    QDir dir = QDir(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    if (!dir.exists()) {
        dir.mkpath(QStandardPaths::writableLocation(QStandardPaths::DataLocation));
    }
    qCDebug(LIBLEARNER_LOG) << "Database path: " << m_databasePath;

    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"));
    db.setDatabaseName(m_databasePath);
    if (!db.open()) {
        qCritical() << "Could not open database: " << db.lastError().text();
        raiseError(db.lastError());
        return db;
    }

    if (!updateSchema()) {
        qCritical() << "Database scheme not correct.";
        return db;
    }

    // return correctly set up database
    return db;
}

bool Storage::updateSchema()
{
    QSqlDatabase db = database();

    // check database version format
    db.exec("CREATE TABLE IF NOT EXISTS metadata ("
            "key TEXT PRIMARY KEY, "
            "value TEXT"
            ")");
    if (db.lastError().isValid()) {
        qCritical() << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }

    QSqlQuery versionQuery = db.exec(QStringLiteral("SELECT value FROM metadata WHERE key = 'version'"));
    if (db.lastError().isValid()) {
        qCritical() << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }

    if (versionQuery.next()) {
        QString version = versionQuery.value(0).toString();
        if (version != QLatin1String("1")) {
            m_errorMessage = i18n("Invalid database version '%1'.", version);
            emit errorMessageChanged();
            return false;
        }
    }
    else {
        if (!db.transaction()) {
            qCWarning(LIBLEARNER_LOG) <<  db.lastError().text();
            raiseError(db.lastError());
            return false;
        }
        db.exec(QStringLiteral("INSERT INTO metadata (key, value) VALUES ('version', '1')"));
        if (db.lastError().isValid()) {
            qCritical() << db.lastError().text();
            raiseError(db.lastError());
            return false;
        }
        if (!db.commit()) {
            qCritical() << db.lastError().text();
            raiseError(db.lastError());
            return false;
        }
    }

    // table for learner profiles
    db.exec("CREATE TABLE IF NOT EXISTS profiles ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "name TEXT"
            ")");
    if (db.lastError().isValid()) {
        qCritical() << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }

    // table for registered learning goals
    db.exec("CREATE TABLE IF NOT EXISTS goals ("
            "category INTEGER, "    // LearningGoal::Category
            "identifier TEXT, "     // identifier, unique per Category
            "name TEXT, "           // name
            "PRIMARY KEY ( category, identifier )"
            ")");
    if (db.lastError().isValid()) {
        qCritical() << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }

    // table for learner - learningGoal relations
    db.exec("CREATE TABLE IF NOT EXISTS learner_goals ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "goal_category INTEGER, "    // LearningGoal::Category
            "goal_identifier TEXT, "     // LearningGoal::Identifier
            "profile_id INTEGER "       // Learner::Identifier
            ")");
    if (db.lastError().isValid()) {
        qCritical() << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }

    // table for full progress data log
    db.exec("CREATE TABLE IF NOT EXISTS learner_progress_log ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "goal_category INTEGER, "    // LearningGoal::Category
            "goal_identifier TEXT, "     // LearningGoal::Identifier
            "profile_id INTEGER, "       // Learner::Identifier
            "item_container TEXT, "
            "item TEXT, "
            "payload INTEGER, "
            "date TEXT"
            ")");
    if (db.lastError().isValid()) {
        qCritical() << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }

    // table for progress data quick access
    db.exec("CREATE TABLE IF NOT EXISTS learner_progress_value ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "goal_category INTEGER, "    // LearningGoal::Category
            "goal_identifier TEXT, "     // LearningGoal::Identifier
            "profile_id INTEGER, "       // Learner::Identifier
            "item_container TEXT, "
            "item TEXT, "
            "payload INTEGER"
            ")");
    if (db.lastError().isValid()) {
        qCritical() << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }

    return true;
}
