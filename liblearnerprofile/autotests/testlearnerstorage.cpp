/*
 *  Copyright 2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "testlearnerstorage.h"
#include "learner.h"
#include "learninggoal.h"
#include "storage.h"

#include <QTest>
#include <QTemporaryFile>

using namespace LearnerProfile;

TestLearnerStorage::TestLearnerStorage()
    : m_storage(nullptr)
{

}

void TestLearnerStorage::init()
{
    QVERIFY(m_db.open());
    m_storage.reset(new Storage(m_db.fileName(), nullptr));
}

void TestLearnerStorage::cleanup()
{
    m_db.close();
}

void TestLearnerStorage::testLearnerStorage()
{
    LearningGoal tmpGoal(LearningGoal::Language, QStringLiteral("testgoalid"), nullptr);
    tmpGoal.setName(QStringLiteral("testgoalname"));

    Learner tmpLearner;
    tmpLearner.addGoal(&tmpGoal);
    tmpLearner.setName(QStringLiteral("tester"));

    QVERIFY(m_storage->storeGoal(&tmpGoal));
    QVERIFY(m_storage->storeProfile(&tmpLearner));

    QList< LearnerProfile::LearningGoal* > loadedGoals = m_storage->loadGoals();
    QCOMPARE(loadedGoals.size(), 1);
    QCOMPARE(loadedGoals.first()->category(), tmpGoal.category());
    QCOMPARE(loadedGoals.first()->identifier(), tmpGoal.identifier());
    QCOMPARE(loadedGoals.first()->name(), tmpGoal.name());

    QList<LearnerProfile::Learner*> loadedLearner = m_storage->loadProfiles(loadedGoals);
    QCOMPARE(loadedLearner.size(), 1);
    QCOMPARE(loadedLearner.first()->identifier(), tmpLearner.identifier());
    QCOMPARE(loadedLearner.first()->name(), tmpLearner.name());
    QCOMPARE(loadedLearner.first()->goals().size(), 1);
    QCOMPARE(loadedLearner.first()->goals().first()->identifier(),
             tmpGoal.identifier());
}

void TestLearnerStorage::testProgressLogStorage()
{
    LearningGoal tmpGoal(LearningGoal::Language, QStringLiteral("testgoalid"), nullptr);
    tmpGoal.setName(QStringLiteral("testgoalname"));

    Learner tmpLearner;
    tmpLearner.addGoal(&tmpGoal);
    tmpLearner.setName(QStringLiteral("tester"));

    QVERIFY(m_storage->storeGoal(&tmpGoal));
    QVERIFY(m_storage->storeProfile(&tmpLearner));

    const QDateTime time{QDateTime::currentDateTime()};
    QVERIFY(m_storage->storeProgressLog(&tmpLearner, &tmpGoal, "container", "item", 1, time));
    auto data = m_storage->readProgressLog(&tmpLearner, &tmpGoal, QStringLiteral("container"), QStringLiteral("item"));
    QCOMPARE(data.size(), 1);
    QCOMPARE(data.first().first.toString(Qt::ISODate), time.toString(Qt::ISODate));
}

void TestLearnerStorage::testProgressValueStorage()
{
    LearningGoal tmpGoal(LearningGoal::Language, QStringLiteral("testgoalid"), nullptr);
    tmpGoal.setName(QStringLiteral("testgoalname"));

    Learner tmpLearner;
    tmpLearner.addGoal(&tmpGoal);
    tmpLearner.setName(QStringLiteral("tester"));

    QVERIFY(m_storage->storeGoal(&tmpGoal));
    QVERIFY(m_storage->storeProfile(&tmpLearner));

    // insert
    QVERIFY(m_storage->storeProgressValue(&tmpLearner, &tmpGoal, "container", "itemA", 1));
    QVERIFY(m_storage->storeProgressValue(&tmpLearner, &tmpGoal, "container", "itemB", 1));

    auto data = m_storage->readProgressValues(&tmpLearner, &tmpGoal, QStringLiteral("container"));
    QCOMPARE(data.size(), 2);

    // update
    QVERIFY(m_storage->storeProgressValue(&tmpLearner, &tmpGoal, "container", "itemA", 2));
    data = m_storage->readProgressValues(&tmpLearner, &tmpGoal, QStringLiteral("container"));
    QCOMPARE(data.find("itemA").value(), 2);
    QCOMPARE(data.find("itemB").value(), 1);
}

QTEST_GUILESS_MAIN(TestLearnerStorage)
