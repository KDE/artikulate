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
#include "core/resources/skeletonresource.h"
#include "../mocks/languagestub.h"

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

void TestSkeletonResource::loadSkeletonResource()
{
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    ResourceRepositoryStub repository({language});
    const QString courseDirectory = "data/contributorrepository/skeletons/";
    const QString courseFile = courseDirectory + "skeleton.xml";

    auto skeleton = SkeletonResource::create(QUrl::fromLocalFile(courseFile), &repository);
    QCOMPARE(skeleton->file().toLocalFile(), courseFile);
    QCOMPARE(skeleton->id(), "skeleton-testdata");
    QCOMPARE(skeleton->foreignId(), "skeleton-testdata"); // always same as ID
    QCOMPARE(skeleton->title(), "Artikulate Test Course Title");
    QCOMPARE(skeleton->description(), "Artikulate Test Course Description");
    QVERIFY(skeleton->language() == nullptr); // a skeleton must not have a language

    QCOMPARE(skeleton->units().count(), 2);

    const auto unit = skeleton->units().first();
    QVERIFY(unit != nullptr);
    QCOMPARE(unit->id(), "{11111111-b885-4833-97ff-27cb1ca2f543}");
    QCOMPARE(unit->title(), QStringLiteral("Numbers"));
    QCOMPARE(unit->phraseList().count(), 2);
    QVERIFY(unit->course() != nullptr);
    QCOMPARE(unit->course(), skeleton.get());

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
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    ResourceRepositoryStub repository({language});
    const QString courseDirectory = "data/contributorrepository/skeletons/";
    const QString courseFile = courseDirectory + "skeleton.xml";
    auto skeleton = SkeletonResource::create(QUrl::fromLocalFile(courseFile), &repository);

    // begin of test
    std::unique_ptr<Unit> unit(new Unit);
    unit->setId("testunit");
    const int initialUnitNumber = skeleton->units().count();
    QCOMPARE(initialUnitNumber, 2);
    QSignalSpy spyAboutToBeAdded(skeleton.get(), SIGNAL(unitAboutToBeAdded(std::shared_ptr<Unit>, int)));
    QSignalSpy spyAdded(skeleton.get(), SIGNAL(unitAdded()));
    QCOMPARE(spyAboutToBeAdded.count(), 0);
    QCOMPARE(spyAdded.count(), 0);
    auto sharedUnit = skeleton->addUnit(std::move(unit));
    QCOMPARE(skeleton->units().count(), initialUnitNumber + 1);
    QCOMPARE(spyAboutToBeAdded.count(), 1);
    QCOMPARE(spyAdded.count(), 1);
    QCOMPARE(sharedUnit->course(), skeleton.get());
}

void TestSkeletonResource::coursePropertyChanges()
{
    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    ResourceRepositoryStub repository({language});
    const QString courseDirectory = "data/contributorrepository/skeletons/";
    const QString courseFile = courseDirectory + "skeleton.xml";
    auto skeleton = SkeletonResource::create(QUrl::fromLocalFile(courseFile), &repository);

    // id
    {
        const QString value = "newId";
        QSignalSpy spy(skeleton.get(), SIGNAL(idChanged()));
        QCOMPARE(spy.count(), 0);
        skeleton->setId(value);
        QCOMPARE(skeleton->id(), value);
        QCOMPARE(spy.count(), 1);
    }

    // title
    {
        const QString value = "newTitle";
        QSignalSpy spy(skeleton.get(), SIGNAL(titleChanged()));
        QCOMPARE(spy.count(), 0);
        skeleton->setTitle(value);
        QCOMPARE(skeleton->title(), value);
        QCOMPARE(spy.count(), 1);
    }

    // description
    {
        const QString value = "newDescription";
        QSignalSpy spy(skeleton.get(), SIGNAL(descriptionChanged()));
        QCOMPARE(spy.count(), 0);
        skeleton->setDescription(value);
        QCOMPARE(skeleton->description(), value);
        QCOMPARE(spy.count(), 1);
    }
}

void TestSkeletonResource::fileLoadSaveCompleteness()
{
    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    ResourceRepositoryStub repository({language});
    const QString courseDirectory = "data/contributorrepository/skeletons/";
    const QString courseFile = courseDirectory + "skeleton.xml";
    auto skeleton = SkeletonResource::create(QUrl::fromLocalFile(courseFile), &repository);

    QTemporaryFile outputFile;
    outputFile.open();
    skeleton->exportToFile(QUrl::fromLocalFile(outputFile.fileName()));

    // note: this only works, since the resource manager not checks uniqueness of course ids!
    auto loadedSkeleton = SkeletonResource::create(QUrl::fromLocalFile(outputFile.fileName()), &repository);

    // test that we actually call the different files
    QVERIFY(skeleton->file().toLocalFile() != loadedSkeleton->file().toLocalFile());
    QCOMPARE(loadedSkeleton->id(), skeleton->id());
    QCOMPARE(loadedSkeleton->foreignId(), skeleton->foreignId());
    QCOMPARE(loadedSkeleton->title(), skeleton->title());
    QCOMPARE(loadedSkeleton->description(), skeleton->description());
    QCOMPARE(loadedSkeleton->language(), skeleton->language());
    QCOMPARE(loadedSkeleton->units().count(), skeleton->units().count());

    auto testUnit = skeleton->units().constFirst();
    auto compareUnit = loadedSkeleton->units().constFirst();
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
