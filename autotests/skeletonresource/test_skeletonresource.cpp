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
    QUrl skeletonFile = QUrl::fromLocalFile(":/artikulate/schemes/skeleton.xsd");
    QXmlSchema skeletonScheme;
    QVERIFY(skeletonScheme.load(skeletonFile));
    QVERIFY(skeletonScheme.isValid());
}

void TestSkeletonResource::loadSkeletonResource()
{
    std::shared_ptr<Language> language(new Language);
    language->setId("de");
    ResourceRepositoryStub repository({language});
    const QString courseDirectory = "data/contributorrepository/skeletons/";
    const QString courseFile = courseDirectory + "skeleton.xml";

    SkeletonResource course(QUrl::fromLocalFile(courseFile), &repository);
    QCOMPARE(course.file().toLocalFile(), courseFile);
    QCOMPARE(course.id(), "skeleton-testdata");
    QCOMPARE(course.foreignId(), "skeleton-testdata"); // always same as ID
    QCOMPARE(course.title(), "Artikulate Test Course Title");
    QCOMPARE(course.description(), "Artikulate Test Course Description");
    QVERIFY(course.language() == nullptr); // a skeleton must not have a language

    QCOMPARE(course.units().count(), 2);

    const auto unit = course.units().first();
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
    std::shared_ptr<Language> language(new Language);
    language->setId("de");
    ResourceRepositoryStub repository({language});
    const QString courseDirectory = "data/contributorrepository/skeletons/";
    const QString courseFile = courseDirectory + "skeleton.xml";
    SkeletonResource course(QUrl::fromLocalFile(courseFile), &repository);

    // begin of test
    std::unique_ptr<Unit> unit(new Unit);
    unit->setId("testunit");
    const int initialUnitNumber = course.units().count();
    QCOMPARE(initialUnitNumber, 2);
    QSignalSpy spyAboutToBeAdded(&course, SIGNAL(unitAboutToBeAdded(Unit*, int)));
    QSignalSpy spyAdded(&course, SIGNAL(unitAdded()));
    QCOMPARE(spyAboutToBeAdded.count(), 0);
    QCOMPARE(spyAdded.count(), 0);
    course.addUnit(std::move(unit));
    QCOMPARE(course.units().count(), initialUnitNumber + 1);
    QCOMPARE(spyAboutToBeAdded.count(), 1);
    QCOMPARE(spyAdded.count(), 1);
}

void TestSkeletonResource::coursePropertyChanges()
{
    // boilerplate
    std::shared_ptr<Language> language(new Language);
    language->setId("de");
    ResourceRepositoryStub repository({language});
    const QString courseDirectory = "data/contributorrepository/skeletons/";
    const QString courseFile = courseDirectory + "skeleton.xml";
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

void TestSkeletonResource::fileLoadSaveCompleteness()
{
    // boilerplate
    std::shared_ptr<Language> language(new Language);
    language->setId("de");
    ResourceRepositoryStub repository({language});
    const QString courseDirectory = "data/contributorrepository/skeletons/";
    const QString courseFile = courseDirectory + "skeleton.xml";
    SkeletonResource course(QUrl::fromLocalFile(courseFile), &repository);

    QTemporaryFile outputFile;
    outputFile.open();
    course.exportCourse(QUrl::fromLocalFile(outputFile.fileName()));

    // note: this only works, since the resource manager not checks uniqueness of course ids!
    SkeletonResource loadedCourse(QUrl::fromLocalFile(outputFile.fileName()), &repository);

    // test that we actually call the different files
    QVERIFY(course.file().toLocalFile() != loadedCourse.file().toLocalFile());
    QCOMPARE(loadedCourse.id(), course.id());
    QCOMPARE(loadedCourse.foreignId(), course.foreignId());
    QCOMPARE(loadedCourse.title(), course.title());
    QCOMPARE(loadedCourse.description(), course.description());
    QCOMPARE(loadedCourse.language(), course.language());
    QCOMPARE(loadedCourse.units().count(), course.units().count());

    auto testUnit = course.units().constFirst();
    auto compareUnit = loadedCourse.units().constFirst();
    QCOMPARE(testUnit->id(), compareUnit->id());
    QCOMPARE(testUnit->foreignId(), compareUnit->foreignId());
    QCOMPARE(testUnit->title(), compareUnit->title());
    QCOMPARE(testUnit->phraseList().count(), compareUnit->phraseList().count());

    Phrase *testPhrase = testUnit->phraseList().constFirst();
    Phrase *comparePhrase = new Phrase(this);
    // note that this actually means that we DO NOT respect phrase orders by list order
    for (Phrase *phrase : compareUnit->phraseList()) {
        if (testPhrase->id() == phrase->id()) {
            comparePhrase = phrase;
            break;
        }
    }
    QVERIFY(testPhrase->id() == comparePhrase->id());
    QVERIFY(testPhrase->foreignId() == comparePhrase->foreignId());
    QVERIFY(testPhrase->text() == comparePhrase->text());
    QVERIFY(testPhrase->type() == comparePhrase->type());
    QVERIFY(testPhrase->sound().fileName() == comparePhrase->sound().fileName());
    QVERIFY(testPhrase->phonemes().count() == comparePhrase->phonemes().count());
}

QTEST_GUILESS_MAIN(TestSkeletonResource)
