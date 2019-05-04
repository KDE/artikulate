/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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

#include "test_courseresource.h"
#include "resourcerepositorystub.h"
#include "core/course.h"
#include "core/language.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "core/resources/languageresource.h"
#include "core/resources/courseresource.h"

#include <QTest>
#include <QDebug>
#include <QTemporaryFile>
#include <QSignalSpy>
#include <QIODevice>
#include <QFile>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QDomDocument>

TestCourseResource::TestCourseResource()
{
}

void TestCourseResource::init()
{
}

void TestCourseResource::cleanup()
{
}

void TestCourseResource::courseSchemeValidationTest()
{
    QUrl schemeFile = QUrl::fromLocalFile(QStringLiteral("schemes/course.xsd"));
    QXmlSchema courseSchema;
    QVERIFY(courseSchema.load(schemeFile));
    QVERIFY(courseSchema.isValid());

    //TODO shall be used in skeleton specific test
    QUrl skeletonFile = QUrl::fromLocalFile(QStringLiteral("schemes/skeleton.xsd"));
    QXmlSchema skeletonScheme;
    QVERIFY(skeletonScheme.load(skeletonFile));
    QVERIFY(skeletonScheme.isValid());
}

void TestCourseResource::loadCourseResource()
{
    Language language;
    language.setId("de");
    ResourceRepositoryStub repository({&language});
    const QString courseDirectory = "data/courses/de/";
    const QString courseFile = courseDirectory + "de.xml";

    CourseResource course(QUrl::fromLocalFile(courseFile), &repository);
    QCOMPARE(course.file().toLocalFile(), courseFile);
    QCOMPARE(course.id(), "de");
    QCOMPARE(course.foreignId(), "artikulate-basic");
    QCOMPARE(course.title(), "Artikulate Deutsch");
    QCOMPARE(course.description(), "Ein Kurs in (hoch-)deutscher Aussprache.");
    QVERIFY(course.language() != nullptr);
    QCOMPARE(course.language()->id(), "de");
    QCOMPARE(course.unitList().count(), 1);

    const auto unit = course.unitList().first();
    QVERIFY(unit != nullptr);
    QCOMPARE(unit->id(), "1");
    QCOMPARE(unit->title(), QStringLiteral("Auf der Straße"));
    QCOMPARE(unit->foreignId(), "{dd60f04a-eb37-44b7-9787-67aaf7d3578d}");

    QCOMPARE(unit->phraseList().count(), 3);
    // note: this test takes the silent assumption that phrases are added to the list in same
    //   order as they are defined in the file. This assumption should be made explicit or dropped
    const auto firstPhrase = unit->phraseList().first();
    QVERIFY(firstPhrase != nullptr);
    QCOMPARE(firstPhrase->id(), "1");
    QCOMPARE(firstPhrase->foreignId(), "{3a4c1926-60d7-44c6-80d1-03165a641c75}");
    QCOMPARE(firstPhrase->text(), "Guten Tag.");
    QCOMPARE(firstPhrase->soundFileUrl(), courseDirectory + "de_01.ogg");
    QCOMPARE(firstPhrase->type(), Phrase::Type::Sentence);
    QVERIFY(firstPhrase->phonemes().isEmpty());
}

void TestCourseResource::addUnitHandling()
{
    // boilerplate
    Language language;
    language.setId("de");
    ResourceRepositoryStub repository({&language});
    const QString courseDirectory = "data/courses/de/";
    const QString courseFile = courseDirectory + "de.xml";
    CourseResource course(QUrl::fromLocalFile(courseFile), &repository);

    // begin of test
    Unit unit;
    unit.setId("testunit");
    const int initialUnitNumber = course.unitList().count();
    QCOMPARE(initialUnitNumber, 1);
    QSignalSpy spyAboutToBeAdded(&course, SIGNAL(unitAboutToBeAdded(Unit*, int)));
    QSignalSpy spyAdded(&course, SIGNAL(unitAdded()));
    QCOMPARE(spyAboutToBeAdded.count(), 0);
    QCOMPARE(spyAdded.count(), 0);
    course.addUnit(&unit);
    QCOMPARE(course.unitList().count(), initialUnitNumber + 1);
    QCOMPARE(spyAboutToBeAdded.count(), 1);
    QCOMPARE(spyAdded.count(), 1);
}

//TODO test signals

// FIXME porting break
void TestCourseResource::fileLoadSaveCompleteness()
{
//    ResourceManager manager;
//    manager.addLanguage(QUrl::fromLocalFile(QStringLiteral("data/languages/de.xml")));
//    manager.addCourse(QUrl::fromLocalFile(QStringLiteral("data/courses/de.xml")));

//    // test to encure further logic
//    QVERIFY(manager.courseResources(manager.languageResources().constFirst()->language()).count() == 1);

//    Course *testCourse = manager.courseResources(manager.languageResources().constFirst()->language()).constFirst()->course();
//    QTemporaryFile outputFile;
//    outputFile.open();
//    QUrl oldFileName = testCourse->file();
//    testCourse->setFile(QUrl::fromLocalFile(outputFile.fileName()));
//    testCourse->setLanguage(manager.languageResources().constFirst()->language());
//    testCourse->sync();
//    testCourse->setFile(oldFileName); // restore for later tests

//    QFile file(outputFile.fileName());
//    if (!file.open(QIODevice::ReadOnly)) {
//        qCritical() << "Could not open file to read.";
//    }

//    //TODO this only works, since the resource manager not checks uniqueness of course ids!
//    manager.addCourse(QUrl::fromLocalFile(outputFile.fileName()));
//    Course *compareCourse = manager.courseResources(manager.languageResources().constFirst()->language()).constLast()->course();

//    // test that we actually call the different files
//    QVERIFY(testCourse->file().toLocalFile() != compareCourse->file().toLocalFile());
//    QVERIFY(testCourse->id() == compareCourse->id());
//    QVERIFY(testCourse->foreignId() == compareCourse->foreignId());
//    QVERIFY(testCourse->title() == compareCourse->title());
//    QVERIFY(testCourse->description() == compareCourse->description());
//    QVERIFY(testCourse->language()->id() == compareCourse->language()->id());
//    QVERIFY(testCourse->unitList().count() == compareCourse->unitList().count());

//    Unit *testUnit = testCourse->unitList().constFirst();
//    Unit *compareUnit = compareCourse->unitList().constFirst();
//    QVERIFY(testUnit->id() == compareUnit->id());
//    QVERIFY(testUnit->foreignId() == compareUnit->foreignId());
//    QVERIFY(testUnit->title() == compareUnit->title());
//    QVERIFY(testUnit->phraseList().count() == compareUnit->phraseList().count());

//    Phrase *testPhrase = testUnit->phraseList().constFirst();
//    Phrase *comparePhrase = new Phrase(this);
//    // Note that this actually means that we DO NOT respect phrase orders by list order!
//    foreach (Phrase *phrase, compareUnit->phraseList()) {
//        if (testPhrase->id() == phrase->id()) {
//            comparePhrase = phrase;
//            break;
//        }
//    }
//    QVERIFY(testPhrase->id() == comparePhrase->id());
//    QVERIFY(testPhrase->foreignId() == comparePhrase->foreignId());
//    QVERIFY(testPhrase->text() == comparePhrase->text());
//    QVERIFY(testPhrase->type() == comparePhrase->type());
//    QVERIFY(testPhrase->sound().fileName() == comparePhrase->sound().fileName());
//    QVERIFY(testPhrase->phonemes().count() == comparePhrase->phonemes().count());
//    //FIXME implement phoneme checks after phonemes are fully implemented
}


QTEST_GUILESS_MAIN(TestCourseResource)
