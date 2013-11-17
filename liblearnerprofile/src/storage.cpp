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

#include "storage.h"
#include "learner.h"

#include <KStandardDirs>
#include <KDebug>
#include <KLocale>

#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>

using namespace LearnerProfile;

Storage::Storage(QObject* parent)
    : QObject(parent)
    , m_errorMessage(QString())
{

}

QString Storage::errorMessage() const
{
    return m_errorMessage;
}

void Storage::raiseError(const QSqlError &error)
{
    m_errorMessage = QString("%1 : %2").arg(error.driverText()).arg(error.databaseText());
    emit errorMessageChanged();
}

bool Storage::storeProfile(Learner *learner)
{
    QSqlDatabase db = database();

    // test whether ID is present
    QSqlQuery idExistsQuery(db);
    idExistsQuery.prepare("SELECT COUNT(*) FROM profiles WHERE id = :id");
    idExistsQuery.bindValue(":id", learner->identifier());
    idExistsQuery.exec();
    if (db.lastError().isValid()) {
        kError() << "ExistsQuery: " << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }
    // go to first result row that contains the count
    idExistsQuery.next();
    if (idExistsQuery.value(0).toInt() < 1) {
        // in case learner ID is not found in database
        QSqlQuery insertProfileQuery(db);
        insertProfileQuery.prepare("INSERT INTO profiles (id, name) VALUES (?, ?)");
        insertProfileQuery.bindValue(0, learner->identifier());
        insertProfileQuery.bindValue(1, learner->name());
        insertProfileQuery.exec();

        if (insertProfileQuery.lastError().isValid()) {
            raiseError(insertProfileQuery.lastError());
            db.rollback();
            return false;
        }
        return true;
    } else {
        // update name otherwise
        QSqlQuery updateProfileQuery(db);
        updateProfileQuery.prepare("UPDATE profiles SET name = :name WHERE id = :id");
        updateProfileQuery.bindValue(":id", learner->identifier());
        updateProfileQuery.bindValue(":name", learner->name());
        updateProfileQuery.exec();
        if (updateProfileQuery.lastError().isValid()) {
            kError() << updateProfileQuery.lastError().text();
            raiseError(updateProfileQuery.lastError());
            db.rollback();
            return false;
        }
        return true;
    }
}

bool Storage::removeProfile(Learner* learner)
{
    QSqlDatabase db = database();
    QSqlQuery removeProfileQuery(db);

    removeProfileQuery.prepare("DELETE FROM profiles WHERE id = ?");
    removeProfileQuery.bindValue(0, learner->identifier());
    removeProfileQuery.exec();

    if (removeProfileQuery.lastError().isValid()) {
        kError() << removeProfileQuery.lastError().text();
        raiseError(removeProfileQuery.lastError());
        db.rollback();
        return false;
    }
    return true;
}

QList< Learner* > Storage::loadProfiles()
{
    QSqlDatabase db = database();
    QSqlQuery profileQuery(db);
    profileQuery.prepare("SELECT id, name FROM profiles");
    profileQuery.exec();
    if (profileQuery.lastError().isValid()) {
        kError() << profileQuery.lastError().text();
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
    return profiles;
}

bool Storage::storeGoal(LearningGoal *goal)
{
    QSqlDatabase db = database();

    // test whether ID is present
    QSqlQuery goalExistsQuery(db);
    goalExistsQuery.prepare("SELECT COUNT(*) FROM goals WHERE category = :category AND identifier = :identifier");
    goalExistsQuery.bindValue(":identifier", goal->identifier());
    goalExistsQuery.bindValue(":category", static_cast<int>(goal->category()));
    goalExistsQuery.exec();
    if (db.lastError().isValid()) {
        kError() << "ExistsQuery: " << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }
    // go to first result row that contains the count
    goalExistsQuery.next();
    if (goalExistsQuery.value(0).toInt() < 1) {
        // in case learner ID is not found in database
        QSqlQuery insertGoalQuery(db);
        insertGoalQuery.prepare("INSERT INTO goals (category, identifier, name) VALUES (?, ?, ?)");
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
        updateGoalQuery.prepare("UPDATE goals SET name = :name WHERE category = :category AND identifier = :identifier");
        updateGoalQuery.bindValue(":category", static_cast<int>(goal->category()));
        updateGoalQuery.bindValue(":identifier", goal->identifier());
        updateGoalQuery.bindValue(":name", goal->name());
        updateGoalQuery.exec();
        if (updateGoalQuery.lastError().isValid()) {
            kError() << updateGoalQuery.lastError().text();
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
    goalQuery.prepare("SELECT category, identifier, name FROM goals");
    goalQuery.exec();
    if (goalQuery.lastError().isValid()) {
        kError() << goalQuery.lastError().text();
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

QSqlDatabase Storage::database()
{
    if (QSqlDatabase::contains(QSqlDatabase::defaultConnection)) {
        return QSqlDatabase::database(QSqlDatabase::defaultConnection);
    }

    QString path = KGlobal::dirs()->locateLocal("appdata", "learnerdata.db");
    kDebug() << "Database path: " << path;

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(path);
    if (!db.open()) {
        kError() << "Could not open database: " << db.lastError().text();
        raiseError(db.lastError());
        return db;
    }

    if (!updateSchema()) {
        kError() << "Database scheme not correct.";
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
        kError() << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }

    QSqlQuery versionQuery = db.exec("SELECT value FROM metadata WHERE key = 'version'");
    if (db.lastError().isValid()) {
        kError() << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }

    if (versionQuery.next()) {
        QString version = versionQuery.value(0).toString();
        if (version != "1") {
            m_errorMessage = i18n("Invalid database version '%1'.", version);
            emit errorMessageChanged();
            return false;
        }
    }
    else {
        if (!db.transaction()) {
            kWarning() <<  db.lastError().text();
            raiseError(db.lastError());
            return false;
        }
        db.exec("INSERT INTO metadata (key, value) VALUES ('version', '1')");
        if (db.lastError().isValid()) {
            kError() << db.lastError().text();
            raiseError(db.lastError());
            return false;
        }
        if (!db.commit()) {
            kError() << db.lastError().text();
            raiseError(db.lastError());
            return false;
        }
    }

    db.exec("CREATE TABLE IF NOT EXISTS profiles ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT, "
            "name TEXT"
            ")");
    if (db.lastError().isValid()) {
        kError() << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }

    db.exec("CREATE TABLE IF NOT EXISTS goals ("
            "category INTEGER, "    // LanguageGoal::Category
            "identifier TEXT, "     // identifier, unique per Category
            "name TEXT, "           // name
            "PRIMARY KEY ( category, identifier )"
            ")");
    if (db.lastError().isValid()) {
        kError() << db.lastError().text();
        raiseError(db.lastError());
        return false;
    }

    return true;
}
