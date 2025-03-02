/*
    SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "testlearnerstorage.h"
#include "learner.h"
#include "learninggoal.h"
#include "storage.h"

#include <QTest>

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

    QList<LearnerProfile::LearningGoal *> loadedGoals = m_storage->loadGoals();
    QCOMPARE(loadedGoals.size(), 1);
    QCOMPARE(loadedGoals.first()->category(), tmpGoal.category());
    QCOMPARE(loadedGoals.first()->identifier(), tmpGoal.identifier());
    QCOMPARE(loadedGoals.first()->name(), tmpGoal.name());

    QList<LearnerProfile::Learner *> loadedLearner = m_storage->loadProfiles(loadedGoals);
    QCOMPARE(loadedLearner.size(), 1);
    QCOMPARE(loadedLearner.first()->identifier(), tmpLearner.identifier());
    QCOMPARE(loadedLearner.first()->name(), tmpLearner.name());
    QCOMPARE(loadedLearner.first()->goals().size(), 1);
    QCOMPARE(loadedLearner.first()->goals().first()->identifier(), tmpGoal.identifier());
    qDeleteAll(loadedGoals);
    qDeleteAll(loadedLearner);
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
    QVERIFY(m_storage->storeProgressLog(&tmpLearner, &tmpGoal, QStringLiteral("container"), QStringLiteral("item"), 1, time));
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
    QVERIFY(m_storage->storeProgressValue(&tmpLearner, &tmpGoal, QStringLiteral("container"), QStringLiteral("itemA"), 1));
    QVERIFY(m_storage->storeProgressValue(&tmpLearner, &tmpGoal, QStringLiteral("container"), QStringLiteral("itemB"), 1));

    auto data = m_storage->readProgressValues(&tmpLearner, &tmpGoal, QStringLiteral("container"));
    QCOMPARE(data.size(), 2);

    // update
    QVERIFY(m_storage->storeProgressValue(&tmpLearner, &tmpGoal, QStringLiteral("container"), QStringLiteral("itemA"), 2));
    data = m_storage->readProgressValues(&tmpLearner, &tmpGoal, QStringLiteral("container"));
    QCOMPARE(data.find(QStringLiteral("itemA")).value(), 2);
    QCOMPARE(data.find(QStringLiteral("itemB")).value(), 1);
}

QTEST_GUILESS_MAIN(TestLearnerStorage)
