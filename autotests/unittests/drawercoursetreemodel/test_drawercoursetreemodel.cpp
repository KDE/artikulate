/*
    SPDX-FileCopyrightText: 2024 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "test_drawercoursetreemodel.h"
#include "../mocks/coursestub.h"
#include "../mocks/languagestub.h"
#include "org/kde/artikulate/drawercoursetreemodel.h"
#include "src/core/icourse.h"
#include "src/core/language.h"
#include "src/core/phrase.h"
#include "src/core/unit.h"
#include <QAbstractItemModelTester>
#include <QSignalSpy>
#include <QTest>

// assumption: during a training session the units and phrases of a course do not change
//   any change of such a course shall result in a reload of a training session

void TestDrawerCourseTreeModel::init()
{
    // no initialization of test case
}

void TestDrawerCourseTreeModel::cleanup()
{
    // no cleanup after test run
}

void TestDrawerCourseTreeModel::createTrainingSessionWithoutUnits()
{
    auto language = std::make_shared<LanguageStub>("de");
    CourseStub course(language, QVector<std::shared_ptr<Unit>>());

    DrawerCourseTreeModel model;
    QAbstractItemModelTester tester(&model);
    model.setCourse(&course);
    QCOMPARE(model.rowCount(), 0); // no units
}

void TestDrawerCourseTreeModel::createTrainingSessionWithBasicUnits()
{
    auto language = std::make_shared<LanguageStub>("de");
    auto unitA = Unit::create();
    auto unitB = Unit::create();
    std::shared_ptr<Phrase> phraseA1 = Phrase::create();
    std::shared_ptr<Phrase> phraseB1 = Phrase::create();
    // note: phrases without soundfiles are skipped in session generation
    phraseA1->setId("A1");
    phraseB1->setId("B1");
    phraseA1->setSound(QUrl::fromLocalFile("/tmp/a1.ogg"));
    phraseB1->setSound(QUrl::fromLocalFile("/tmp/b1.ogg"));
    unitA->addPhrase(phraseA1, unitA->phrases().size());
    unitB->addPhrase(phraseB1, unitB->phrases().size());

    CourseStub course(language, QVector<std::shared_ptr<Unit>>({unitA, unitB}));

    DrawerCourseTreeModel model;
    QAbstractItemModelTester tester(&model);
    model.setCourse(&course);
    QCOMPARE(model.rowCount(), 2); // units
}

void TestDrawerCourseTreeModel::createTrainingSessionWithEmptySounds()
{
    auto language = std::make_shared<LanguageStub>("de");
    auto unitA = Unit::create();
    auto unitB = Unit::create();
    std::shared_ptr<Phrase> phraseA1 = Phrase::create();
    std::shared_ptr<Phrase> phraseA2 = Phrase::create();
    std::shared_ptr<Phrase> phraseB1 = Phrase::create();
    // note: phrases without soundfiles are skipped in session generation
    phraseA1->setId("A1");
    phraseA2->setId("A2");
    phraseB1->setId("B1");
    phraseA1->setSound(QUrl::fromLocalFile("/tmp/a1.ogg"));
    unitA->addPhrase(phraseA1, unitA->phrases().size());
    unitA->addPhrase(phraseA2, unitA->phrases().size());
    unitB->addPhrase(phraseB1, unitB->phrases().size());

    CourseStub course(language, QList<std::shared_ptr<Unit>>({unitA, unitB}));
    DrawerCourseTreeModel model;
    QAbstractItemModelTester tester(&model);
    model.setCourse(&course);

    // test number of actions
    model.setTrainingFilter(true);
    QCOMPARE(model.rowCount(QModelIndex()), 1);
    QCOMPARE(model.rowCount(model.index(0, 0)), 1);

    model.setTrainingFilter(false);
    QCOMPARE(model.rowCount(QModelIndex()), 2);
    QCOMPARE(model.rowCount(model.index(0, 0)), 2);
    QCOMPARE(model.rowCount(model.index(1, 0)), 1);
}

void TestDrawerCourseTreeModel::createTrainingSessionWithEmptyUnits()
{
    auto language = std::make_shared<LanguageStub>("de");
    auto unitA = Unit::create();
    auto unitB = Unit::create();
    CourseStub course(language, QVector<std::shared_ptr<Unit>>({unitA, unitB}));
    DrawerCourseTreeModel model;
    QAbstractItemModelTester tester(&model);
    model.setCourse(&course);

    model.setTrainingFilter(true);
    QCOMPARE(model.rowCount(QModelIndex()), 0);

    model.setTrainingFilter(false);
    QCOMPARE(model.rowCount(QModelIndex()), 2);
}

void TestDrawerCourseTreeModel::iterateCourse()
{
    auto language = std::make_shared<LanguageStub>("de");
    auto unitA = Unit::create();
    auto unitB = Unit::create();
    std::shared_ptr<Phrase> phraseA1 = Phrase::create();
    std::shared_ptr<Phrase> phraseA2 = Phrase::create();
    std::shared_ptr<Phrase> phraseA3 = Phrase::create();
    std::shared_ptr<Phrase> phraseB1 = Phrase::create();
    std::shared_ptr<Phrase> phraseB2 = Phrase::create();
    // note: phrases without soundfiles are skipped in session generation
    phraseA1->setId("A1");
    phraseA2->setId("A2");
    phraseA3->setId("A3");
    phraseB1->setId("B1");
    phraseB2->setId("B2");
    phraseA1->setText("A1 text");
    phraseA2->setText("A2 text");
    phraseB1->setText("B1 text");
    phraseB2->setText("B2 text");
    phraseA1->setSound(QUrl::fromLocalFile("/tmp/a1.ogg"));
    phraseA2->setSound(QUrl::fromLocalFile("/tmp/a1.ogg"));
    // no sound for phrase A3
    phraseB1->setSound(QUrl::fromLocalFile("/tmp/b1.ogg"));
    phraseB2->setSound(QUrl::fromLocalFile("/tmp/b2.ogg"));
    unitA->addPhrase(phraseA1, unitA->phrases().size());
    unitA->addPhrase(phraseA2, unitA->phrases().size());
    unitA->addPhrase(phraseA3, unitA->phrases().size());
    unitB->addPhrase(phraseB1, unitB->phrases().size());
    unitB->addPhrase(phraseB2, unitB->phrases().size());

    CourseStub course(language, QVector<std::shared_ptr<Unit>>({unitA, unitB}));
    DrawerCourseTreeModel model;
    QAbstractItemModelTester tester(&model);
    model.setCourse(&course);

    const QList<QString> testPhrases = {"A1 text", "A2 text", "B1 text", "B2 text"};
    QList<QString> iteratedPhrases;
    QModelIndex index;
    do {
        index = model.next(index);
        if (index.isValid()) {
            iteratedPhrases << model.data(index, Qt::DisplayRole).toString();
        }
    } while (index.isValid());

    QCOMPARE(iteratedPhrases, testPhrases);
}

QTEST_GUILESS_MAIN(TestDrawerCourseTreeModel)
