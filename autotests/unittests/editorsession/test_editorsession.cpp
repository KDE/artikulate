/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "test_editorsession.h"
#include "../mocks/editablecoursestub.h"
#include "../mocks/languagestub.h"
#include "editablerepositorystub.h"
#include "src/core/editorsession.h"
#include "src/core/icourse.h"
#include "src/core/ieditablecourse.h"
#include "src/core/ieditablerepository.h"
#include "src/core/language.h"
#include "src/core/resources/skeletonresource.h"
#include "src/core/trainingaction.h"
#include "src/core/unit.h"
#include <QSignalSpy>
#include <QTest>
#include <QVector>

void TestEditorSession::init()
{
    // no initialization of test case
}

void TestEditorSession::cleanup()
{
    // no cleanup after test run
}

void TestEditorSession::createEditorSession()
{
    auto languageGerman = std::make_shared<LanguageStub>("de");
    auto languageEnglish = std::make_shared<LanguageStub>("en");

    std::shared_ptr<IEditableCourse> course = EditableCourseStub::create(languageGerman, QVector<std::shared_ptr<Unit>>());
    course->setLanguage(languageGerman);
    auto skeleton = SkeletonResource::create(QUrl(), nullptr);

    EditableRepositoryStub repository {
        {languageGerman, languageEnglish}, // languages
        {skeleton},                        // skeletons
        {course}                           // courses
    };
    EditorSession session;
    session.setRepository(&repository);
    QVERIFY(session.course() == nullptr);
    QVERIFY(session.language() == nullptr);
}

void TestEditorSession::nonSkeletonSwitchingBehavior()
{
    auto languageGerman = std::make_shared<LanguageStub>("de");
    auto languageEnglish = std::make_shared<LanguageStub>("en");
    std::shared_ptr<IEditableCourse> courseGerman = EditableCourseStub::create(languageGerman, QVector<std::shared_ptr<Unit>>());
    courseGerman->setId("course-german");
    std::shared_ptr<IEditableCourse> courseEnglish = EditableCourseStub::create(languageEnglish, QVector<std::shared_ptr<Unit>>());
    courseEnglish->setId("course-english");

    EditableRepositoryStub repository {
        {languageGerman, languageEnglish}, // languages
        {},                                // skeletons
        {courseGerman, courseEnglish}      // courses
    };
    EditorSession session;
    session.setRepository(&repository);

    QVERIFY(session.course() == nullptr);
    session.setCourse(courseGerman.get());
    QCOMPARE(session.course()->id(), courseGerman->id());
    QVERIFY(session.language() != nullptr);
    QCOMPARE(session.language()->id(), languageGerman->id());

    QVERIFY(session.language() != nullptr);
    QCOMPARE(session.language()->id(), languageGerman->id());
    session.setCourse(courseEnglish.get());
    QVERIFY(session.course() != nullptr);
    QCOMPARE(session.course()->id(), courseEnglish->id());
    QVERIFY(session.language() != nullptr);
    QCOMPARE(session.language()->id(), languageEnglish->id());
}

void TestEditorSession::skeletonSwitchingBehavior()
{
    auto languageGerman = std::make_shared<LanguageStub>("de");
    auto languageEnglish = std::make_shared<LanguageStub>("en");
    std::shared_ptr<IEditableCourse> courseGermanA = EditableCourseStub::create(languageGerman, QVector<std::shared_ptr<Unit>>());
    courseGermanA->setId("course-german");
    courseGermanA->setForeignId("testskeletonA");
    std::shared_ptr<IEditableCourse> courseGermanB = EditableCourseStub::create(languageGerman, QVector<std::shared_ptr<Unit>>());
    courseGermanB->setId("course-german");
    courseGermanB->setForeignId("testskeletonB");
    std::shared_ptr<IEditableCourse> courseEnglishA = EditableCourseStub::create(languageEnglish, QVector<std::shared_ptr<Unit>>());
    courseEnglishA->setId("course-english");
    courseEnglishA->setForeignId("testskeletonA");
    auto skeletonA = SkeletonResource::create(QUrl(), nullptr);
    skeletonA->setId("testskeletonA");
    auto skeletonB = SkeletonResource::create(QUrl(), nullptr);
    skeletonB->setId("testskeletonB");

    EditableRepositoryStub repository {
        {languageGerman, languageEnglish},             // languages
        {skeletonA, skeletonB},                        // skeletons
        {courseGermanA, courseEnglishA, courseGermanB} // courses
    };
    EditorSession session;
    session.setRepository(&repository);

    session.setCourse(courseGermanA.get());
    Q_ASSERT(session.course() != nullptr);
    QCOMPARE(session.course()->id(), courseGermanA->id());
    session.setCourse(courseEnglishA.get());
    Q_ASSERT(session.course() != nullptr);
    QCOMPARE(session.course()->id(), courseEnglishA->id());

    session.setCourse(courseGermanB.get());
    QVERIFY(session.course() != nullptr);
    QCOMPARE(session.course()->id(), courseGermanB->id());
    QVERIFY(session.language() != nullptr);
    QCOMPARE(session.language()->id(), languageGerman->id());
}

void TestEditorSession::iterateCourse()
{
    // language
    auto language = std::make_shared<LanguageStub>("de");

    // course
    auto unitA = Unit::create();
    auto unitB = Unit::create();
    std::shared_ptr<Phrase> phraseA1 = Phrase::create();
    std::shared_ptr<Phrase> phraseA2 = Phrase::create();
    std::shared_ptr<Phrase> phraseB1 = Phrase::create();
    std::shared_ptr<Phrase> phraseB2 = Phrase::create();
    // note: phrases without soundfiles are skipped in session generation
    phraseA1->setId("A1");
    phraseA2->setId("A2");
    phraseB1->setId("B1");
    phraseB2->setId("B2");
    phraseA1->setSound(QUrl::fromLocalFile("/tmp/a1.ogg"));
    phraseA2->setSound(QUrl::fromLocalFile("/tmp/a1.ogg"));
    phraseB1->setSound(QUrl::fromLocalFile("/tmp/b1.ogg"));
    phraseB2->setSound(QUrl::fromLocalFile("/tmp/b2.ogg"));
    unitA->addPhrase(phraseA1, unitA->phrases().size());
    unitA->addPhrase(phraseA2, unitA->phrases().size());
    unitB->addPhrase(phraseB1, unitB->phrases().size());
    unitB->addPhrase(phraseB2, unitB->phrases().size());
    auto course = EditableCourseStub::create(language, QVector<std::shared_ptr<Unit>>({unitA, unitB}));

    EditableRepositoryStub repository {
        {language}, // languages
        {},         // skeletons
        {course}    // courses
    };
    EditorSession session;
    session.setRepository(&repository);
    session.setCourse(course.get());

    // session assumed to initialize with first units's first phrase
    QCOMPARE(session.activeUnit(), unitA.get());
    QCOMPARE(session.activePhrase()->self(), phraseA1);
    QVERIFY(course.get() == session.course());

    // test direct unit setters
    session.setActiveUnit(unitA.get());
    QCOMPARE(session.activeUnit()->self(), unitA);
    session.setActiveUnit(unitB.get());
    QCOMPARE(session.activeUnit()->self(), unitB);

    // test direct phrase setters
    session.setActivePhrase(phraseA1.get());
    QCOMPARE(session.activePhrase()->self(), phraseA1);
    QCOMPARE(session.activeUnit()->self(), unitA);
    session.setActivePhrase(phraseB1.get());
    QCOMPARE(session.activePhrase()->self(), phraseB1);
    QCOMPARE(session.activeUnit()->self(), unitB);

    // test phrase forward iterators
    session.setActivePhrase(phraseA1.get());
    QCOMPARE(session.activeUnit()->self(), unitA);
    QCOMPARE(session.activePhrase()->id(), phraseA1->id());
    QVERIFY(session.hasNextPhrase());
    session.switchToNextPhrase();
    QCOMPARE(session.activeUnit()->self(), unitA);
    QCOMPARE(session.activePhrase()->id(), phraseA2->id());
    session.switchToNextPhrase();
    QCOMPARE(session.activePhrase()->self(), phraseB1);
    session.switchToNextPhrase();
    QCOMPARE(session.activePhrase()->self(), phraseB2);
    QVERIFY(!session.hasNextPhrase());

    // at the end, do not iterate further
    session.switchToNextPhrase();
    QCOMPARE(session.activePhrase()->self(), phraseB2);

    // test phrase backward iterators
    QVERIFY(session.hasPreviousPhrase());
    session.switchToPreviousPhrase();
    QCOMPARE(session.activePhrase()->self(), phraseB1);
    session.switchToPreviousPhrase();
    QCOMPARE(session.activePhrase()->id(), phraseA2->id());
    session.switchToPreviousPhrase();
    QCOMPARE(session.activePhrase()->id(), phraseA1->id());
    QVERIFY(!session.hasPreviousPhrase());

    // at the end, do not iterate further
    session.switchToPreviousPhrase();
    QCOMPARE(session.activePhrase()->id(), phraseA1->id());
}

void TestEditorSession::updateActionsBehavior()
{
    // test preparation
    auto language = std::make_shared<LanguageStub>("de");
    auto unitA = Unit::create();
    auto unitB = Unit::create();
    unitA->setTitle("titleA");
    unitB->setTitle("titleB");
    std::shared_ptr<Phrase> phraseA1 = Phrase::create();
    std::shared_ptr<Phrase> phraseA2 = Phrase::create();
    std::shared_ptr<Phrase> phraseB1 = Phrase::create();
    std::shared_ptr<Phrase> phraseB2 = Phrase::create();
    phraseA1->setId("A1");
    phraseA2->setId("A2");
    phraseB1->setId("B1");
    phraseB2->setId("B2");
    phraseA1->setText("A1");
    phraseA2->setText("A2");
    phraseB1->setText("B1");
    phraseB2->setText("B2");
    unitA->addPhrase(phraseA1, unitA->phrases().size());
    unitA->addPhrase(phraseA2, unitA->phrases().size());
    unitB->addPhrase(phraseB1, unitB->phrases().size());
    unitB->addPhrase(phraseB2, unitB->phrases().size());
    auto course = EditableCourseStub::create(language, QVector<std::shared_ptr<Unit>>({unitA, unitB}));

    EditableRepositoryStub repository {
        {language}, // languages
        {},         // skeletons
        {course}    // courses
    };
    EditorSession session;
    session.setRepository(&repository);
    QVERIFY(session.activeAction() == nullptr);
    QCOMPARE(session.trainingActions().count(), 0);

    {
        QSignalSpy spy(&session, &EditorSession::actionsChanged);
        session.setCourse(course.get()); // setting the course shall trigger the action update
        QVERIFY(spy.count() > 0);
    }
    QVERIFY(session.activeAction() != nullptr);

    QCOMPARE(session.trainingActions().count(), 2); // 2 units
    QCOMPARE(session.trainingActions().at(0)->text(), "titleA");
    QCOMPARE(session.trainingActions().at(1)->text(), "titleB");
    QCOMPARE(session.trainingActions().at(0)->actions().count(), 2);
    QCOMPARE(session.trainingActions().at(1)->actions().count(), 2);

    auto phraseA1Object = qobject_cast<TrainingAction *>(session.trainingActions().at(0)->actions().first());
    QVERIFY(phraseA1Object != nullptr);
    QCOMPARE(phraseA1Object->text(), phraseA1->text());

    // test update of unit
    {
        QCOMPARE(session.trainingActions().last()->text(), unitB->title()); // ensure that correct action is selected
        QSignalSpy spy(session.trainingActions().last(), &TrainingAction::actionsChanged);
        unitB->removePhrase(phraseB1->self()); // note: event has to be trigger explicitly
        course->triggerUnitChanged(unitB);
        QVERIFY(spy.count() > 0);
    }
}

QTEST_GUILESS_MAIN(TestEditorSession)
