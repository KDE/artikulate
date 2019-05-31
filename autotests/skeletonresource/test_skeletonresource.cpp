/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "test_skeletonresource.h"
#include "resourcerepositorystub.h"
#include "core/language.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "core/resources/languageresource.h"
#include "core/resources/skeletonresource.h"

#include <QTest>
#include <QDebug>
#include <QTemporaryFile>
#include <QSignalSpy>
#include <QIODevice>
#include <QFile>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QDomDocument>

TestSkeletonResource::TestSkeletonResource()
{
}

void TestSkeletonResource::init()
{
}

void TestSkeletonResource::cleanup()
{
}

void TestSkeletonResource::schemeValidationTest()
{
    QUrl skeletonFile = QUrl::fromLocalFile(QStringLiteral("schemes/skeleton.xsd"));
    QXmlSchema skeletonScheme;
    QVERIFY(skeletonScheme.load(skeletonFile));
    QVERIFY(skeletonScheme.isValid());
}

void TestSkeletonResource::loadSkeletonResource()
{
    Language language;
    language.setId("de");
    ResourceRepositoryStub repository({&language});
    const QString courseDirectory = "data/contributorrepository/skeletons/";
    const QString courseFile = courseDirectory + "skeleton.xml";

    SkeletonResource course(QUrl::fromLocalFile(courseFile), &repository);
    QCOMPARE(course.file().toLocalFile(), courseFile);
    QCOMPARE(course.id(), "skeleton-testdata");
    QCOMPARE(course.foreignId(), "skeleton-testdata"); // always same as ID
    QCOMPARE(course.title(), "Artikulate Test Course Title");
    QCOMPARE(course.description(), "Artikulate Test Course Description");
    QVERIFY(course.language() == nullptr); // a skeleton must not have a language

    QCOMPARE(course.unitList().count(), 2);

    const auto unit = course.unitList().first();
    QVERIFY(unit != nullptr);
    QCOMPARE(unit->id(), "{11111111-b885-4833-97ff-27cb1ca2f543}");
    QCOMPARE(unit->title(), QStringLiteral("Numbers"));
    QCOMPARE(unit->phraseList().count(), 2);
    // note: this test takes the silent assumption that phrases are added to the list in same
    //   order as they are defined in the file. This assumption should be made explicit or dropped
    const auto firstPhrase = unit->phraseList().first();
    QVERIFY(firstPhrase != nullptr);
    QCOMPARE(firstPhrase->id(), "{22222222-9234-4da5-a6fe-dbd5104f57d5}");
    QCOMPARE(firstPhrase->text(), "0");
    QCOMPARE(firstPhrase->type(), Phrase::Type::Word);
    const auto secondPhrase = unit->phraseList().at(1);
    QVERIFY(secondPhrase != nullptr);
    QCOMPARE(secondPhrase->id(), "{333333333-b4a9-4264-9a26-75a55aa5d302}");
    QCOMPARE(secondPhrase->text(), "1");
    QCOMPARE(secondPhrase->type(), Phrase::Type::Word);
}

void TestSkeletonResource::unitAddAndRemoveHandling()
{
    // boilerplate
    Language language;
    language.setId("de");
    ResourceRepositoryStub repository({&language});
    const QString courseDirectory = "data/courses/de/";
    const QString courseFile = courseDirectory + "de.xml";
    SkeletonResource course(QUrl::fromLocalFile(courseFile), &repository);

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

void TestSkeletonResource::coursePropertyChanges()
{
    // boilerplate
    Language language;
    language.setId("de");
    ResourceRepositoryStub repository({&language});
    const QString courseDirectory = "data/courses/de/";
    const QString courseFile = courseDirectory + "de.xml";
    SkeletonResource course(QUrl::fromLocalFile(courseFile), &repository);

    // id
    {
        const QString value = "newId";
        QSignalSpy spy(&course, SIGNAL(idChanged()));
        QCOMPARE(spy.count(), 0);
        course.setId(value);
        QCOMPARE(course.id(), value);
        QCOMPARE(spy.count(), 1);
    }

    // title
    {
        const QString value = "newTitle";
        QSignalSpy spy(&course, SIGNAL(titleChanged()));
        QCOMPARE(spy.count(), 0);
        course.setTitle(value);
        QCOMPARE(course.title(), value);
        QCOMPARE(spy.count(), 1);
    }

    // description
    {
        const QString value = "newDescription";
        QSignalSpy spy(&course, SIGNAL(descriptionChanged()));
        QCOMPARE(spy.count(), 0);
        course.setDescription(value);
        QCOMPARE(course.description(), value);
        QCOMPARE(spy.count(), 1);
    }
}

// FIXME porting break
//void TestCourseResource::fileLoadSaveCompleteness()
//{
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
//}


QTEST_GUILESS_MAIN(TestSkeletonResource)
