/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "test_editablecourseresource.h"
#include "../mocks/coursestub.h"
#include "../mocks/languagestub.h"
#include "core/language.h"
#include "core/phrase.h"
#include "core/resources/courseparser.h"
#include "core/resources/editablecourseresource.h"
#include "core/unit.h"
#include "resourcerepositorystub.h"

#include <QDebug>
#include <QDomDocument>
#include <QFile>
#include <QIODevice>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTest>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <memory>

TestEditableCourseResource::TestEditableCourseResource()
{
    qRegisterMetaType<std::shared_ptr<IEditableUnit>>("std::shared_ptr<IEditableUnit>");
}

void TestEditableCourseResource::init()
{
}

void TestEditableCourseResource::cleanup()
{
}

void TestEditableCourseResource::loadCourseResource()
{
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    ResourceRepositoryStub repository({language});

    auto course = EditableCourseResource::create(QUrl::fromLocalFile(":/courses/de.xml"), &repository);
    QCOMPARE(course->file().toLocalFile(), ":/courses/de.xml");
    QCOMPARE(course->id(), "de");
    QCOMPARE(course->foreignId(), "artikulate-basic");
    QCOMPARE(course->title(), "Artikulate Deutsch");
    QCOMPARE(course->description(), "Ein Kurs in (hoch-)deutscher Aussprache.");
    QVERIFY(course->language() != nullptr);
    QCOMPARE(course->language()->id(), "de");
    QCOMPARE(course->units().count(), 1);
    QCOMPARE(course->units().first()->course(), course);

    const auto unit = course->units().first();
    QVERIFY(unit != nullptr);
    QCOMPARE(unit->id(), "1");
    QCOMPARE(unit->title(), QStringLiteral("Auf der Straße"));
    QCOMPARE(unit->foreignId(), "{dd60f04a-eb37-44b7-9787-67aaf7d3578d}");
    QCOMPARE(unit->course(), course);

    QCOMPARE(unit->phrases().count(), 3);
    // note: this test takes the silent assumption that phrases are added to the list in same
    //   order as they are defined in the file. This assumption should be made explicit or dropped
    const auto firstPhrase = unit->phrases().first();
    QVERIFY(firstPhrase != nullptr);
    QCOMPARE(firstPhrase->id(), "1");
    QCOMPARE(firstPhrase->foreignId(), "{3a4c1926-60d7-44c6-80d1-03165a641c75}");
    QCOMPARE(firstPhrase->text(), "Guten Tag.");
    QCOMPARE(firstPhrase->soundFileUrl(), ":/courses/de_01.ogg");
    QCOMPARE(firstPhrase->type(), Phrase::Type::Sentence);
    QVERIFY(firstPhrase->phonemes().isEmpty());
}

void TestEditableCourseResource::unitAddAndRemoveHandling()
{
    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    ResourceRepositoryStub repository({language});
    auto course = EditableCourseResource::create(QUrl::fromLocalFile(":/courses/de.xml"), &repository);

    // begin of test
    auto unit = Unit::create();
    unit->setId("testunit");
    const int initialUnitNumber = course->units().count();
    QCOMPARE(initialUnitNumber, 1);
    QSignalSpy spyAboutToBeAdded(course.get(), SIGNAL(unitAboutToBeAdded(std::shared_ptr<Unit>, int)));
    QSignalSpy spyAdded(course.get(), SIGNAL(unitAdded()));
    QCOMPARE(spyAboutToBeAdded.count(), 0);
    QCOMPARE(spyAdded.count(), 0);
    auto sharedUnit = course->addUnit(std::move(unit));
    QCOMPARE(course->units().count(), initialUnitNumber + 1);
    QCOMPARE(spyAboutToBeAdded.count(), 1);
    QCOMPARE(spyAdded.count(), 1);
    QCOMPARE(sharedUnit->course(), course);
}

void TestEditableCourseResource::phraseAddAndRemoveHandling()
{
    //TODO simplify test by using empty course

    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    ResourceRepositoryStub repository({language});
    auto course = EditableCourseResource::create(QUrl::fromLocalFile(":/courses/de.xml"), &repository);
    auto unit = Unit::create();
    unit->setId("testunit");
    course->addUnit(unit);

    auto testPhrase = Phrase::create();
    testPhrase->setId("testphrase");
    unit->addPhrase(testPhrase, 0);
    QCOMPARE(unit->phrases().count(), 1);
    QCOMPARE(testPhrase->unit()->id(), "testunit");

    // begin of test
    {
        QSignalSpy spy(course.get(), &IEditableCourse::unitChanged);
        QVERIFY(course->createPhraseAfter(testPhrase.get()));
        QCOMPARE(spy.count(), 1);
        QCOMPARE(unit->phrases().count(), 2);
    }
    {
        QSignalSpy spy(course.get(), &IEditableCourse::unitChanged);
        QVERIFY(course->deletePhrase(testPhrase.get()));
        QCOMPARE(spy.count(), 1);
        QCOMPARE(unit->phrases().count(), 1);
    }
}

void TestEditableCourseResource::coursePropertyChanges()
{
    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    ResourceRepositoryStub repository({language});
    auto course = CourseResource::create(QUrl::fromLocalFile(":/courses/de.xml"), &repository);

    // id
    {
        const QString value = "newId";
        QSignalSpy spy(course.get(), &CourseResource::idChanged);
        QCOMPARE(spy.count(), 0);
        course->setId(value);
        QCOMPARE(course->id(), value);
        QCOMPARE(spy.count(), 1);
    }

    // foreign id
    {
        const QString value = "newForeignId";
        QSignalSpy spy(course.get(), SIGNAL(foreignIdChanged()));
        QCOMPARE(spy.count(), 0);
        course->setForeignId(value);
        QCOMPARE(course->foreignId(), value);
        QCOMPARE(spy.count(), 1);
    }

    // title
    {
        const QString value = "newTitle";
        QSignalSpy spy(course.get(), SIGNAL(titleChanged()));
        QCOMPARE(spy.count(), 0);
        course->setTitle(value);
        QCOMPARE(course->title(), value);
        QCOMPARE(spy.count(), 1);
    }

    // title
    {
        const QString value = "newI18nTitle";
        QSignalSpy spy(course.get(), SIGNAL(i18nTitleChanged()));
        QCOMPARE(spy.count(), 0);
        course->setI18nTitle(value);
        QCOMPARE(course->i18nTitle(), value);
        QCOMPARE(spy.count(), 1);
    }

    // description
    {
        const QString value = "newDescription";
        QSignalSpy spy(course.get(), SIGNAL(descriptionChanged()));
        QCOMPARE(spy.count(), 0);
        course->setDescription(value);
        QCOMPARE(course->description(), value);
        QCOMPARE(spy.count(), 1);
    }

    // language
    {
        std::shared_ptr<Language> testLanguage;
        QSignalSpy spy(course.get(), SIGNAL(languageChanged()));
        QCOMPARE(spy.count(), 0);
        course->setLanguage(testLanguage);
        QCOMPARE(course->language(), testLanguage);
        QCOMPARE(spy.count(), 1);
    }
}

void TestEditableCourseResource::fileLoadSaveCompleteness()
{
    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    ResourceRepositoryStub repository({language});
    auto course = EditableCourseResource::create(QUrl::fromLocalFile(":/courses/de.xml"), &repository);

    QTemporaryFile outputFile;
    outputFile.open();
    course->exportToFile(QUrl::fromLocalFile(outputFile.fileName()));
    // note: this only works, since the resource manager not checks uniqueness of course ids!
    auto loadedCourse = EditableCourseResource::create(QUrl::fromLocalFile(outputFile.fileName()), &repository);

    // test that we actually call the different files
    QVERIFY(course->file().toLocalFile() != loadedCourse->file().toLocalFile());
    QVERIFY(course->id() == loadedCourse->id());
    QVERIFY(course->foreignId() == loadedCourse->foreignId());
    QVERIFY(course->title() == loadedCourse->title());
    QVERIFY(course->description() == loadedCourse->description());
    QVERIFY(course->language()->id() == loadedCourse->language()->id());
    QCOMPARE(loadedCourse->units().count(), course->units().count());

    auto testUnit = course->units().constFirst();
    auto compareUnit = loadedCourse->units().constFirst();
    QCOMPARE(compareUnit->id(), testUnit->id());
    QCOMPARE(compareUnit->foreignId(), testUnit->foreignId());
    QCOMPARE(compareUnit->title(), testUnit->title());
    QCOMPARE(compareUnit->phrases().count(), testUnit->phrases().count());

    std::shared_ptr<IPhrase> testPhrase = testUnit->phrases().constFirst();
    std::shared_ptr<IPhrase> comparePhrase = Phrase::create();
    // note that this actually means that we DO NOT respect phrase orders by list order
    for (const auto &phrase : compareUnit->phrases()) {
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

void TestEditableCourseResource::modifiedStatus()
{
    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    ResourceRepositoryStub repository({language});
    auto course = EditableCourseResource::create(QUrl::fromLocalFile(":/courses/de.xml"), &repository);

    { // initial file loading
        QTemporaryFile outputFile;
        outputFile.open();
        course->exportToFile(QUrl::fromLocalFile(outputFile.fileName()));
        auto loadedCourse = EditableCourseResource::create(QUrl::fromLocalFile(outputFile.fileName()), &repository);
        QCOMPARE(loadedCourse->isModified(), false);
    }

    { // set ID
        QTemporaryFile outputFile;
        outputFile.open();
        course->exportToFile(QUrl::fromLocalFile(outputFile.fileName()));
        auto loadedCourse = EditableCourseResource::create(QUrl::fromLocalFile(outputFile.fileName()), &repository);
        QCOMPARE(loadedCourse->isModified(), false);
        loadedCourse->setId("ASDF");
        QCOMPARE(loadedCourse->isModified(), true);
        loadedCourse->sync();
        QCOMPARE(loadedCourse->isModified(), false);
    }

    { // set title
        QTemporaryFile outputFile;
        outputFile.open();
        course->exportToFile(QUrl::fromLocalFile(outputFile.fileName()));
        auto loadedCourse = EditableCourseResource::create(QUrl::fromLocalFile(outputFile.fileName()), &repository);
        QCOMPARE(loadedCourse->isModified(), false);
        loadedCourse->setTitle("ASDF");
        QCOMPARE(loadedCourse->isModified(), true);
        loadedCourse->sync();
        QCOMPARE(loadedCourse->isModified(), false);
    }

    { // set i18n title
        QTemporaryFile outputFile;
        outputFile.open();
        course->exportToFile(QUrl::fromLocalFile(outputFile.fileName()));
        auto loadedCourse = EditableCourseResource::create(QUrl::fromLocalFile(outputFile.fileName()), &repository);
        QCOMPARE(loadedCourse->isModified(), false);
        loadedCourse->setI18nTitle("ASDF");
        QCOMPARE(loadedCourse->isModified(), true);
        loadedCourse->sync();
        QCOMPARE(loadedCourse->isModified(), false);
    }

    { // set description
        QTemporaryFile outputFile;
        outputFile.open();
        course->exportToFile(QUrl::fromLocalFile(outputFile.fileName()));
        auto loadedCourse = EditableCourseResource::create(QUrl::fromLocalFile(outputFile.fileName()), &repository);
        QCOMPARE(loadedCourse->isModified(), false);
        loadedCourse->setDescription("ASDF");
        QCOMPARE(loadedCourse->isModified(), true);
        loadedCourse->sync();
        QCOMPARE(loadedCourse->isModified(), false);
    }

    { // set language
        QTemporaryFile outputFile;
        outputFile.open();
        course->exportToFile(QUrl::fromLocalFile(outputFile.fileName()));
        auto loadedCourse = EditableCourseResource::create(QUrl::fromLocalFile(outputFile.fileName()), &repository);
        QCOMPARE(loadedCourse->isModified(), false);
        std::shared_ptr<ILanguage> newLanguage(new LanguageStub("en"));
        loadedCourse->setLanguage(newLanguage);
        QCOMPARE(loadedCourse->isModified(), true);
        loadedCourse->sync();
        QCOMPARE(loadedCourse->isModified(), false);
    }

    { // add unit
        QTemporaryFile outputFile;
        outputFile.open();
        course->exportToFile(QUrl::fromLocalFile(outputFile.fileName()));
        auto loadedCourse = EditableCourseResource::create(QUrl::fromLocalFile(outputFile.fileName()), &repository);
        QCOMPARE(loadedCourse->isModified(), false);
        auto unit = Unit::create();
        unit->setId("testunit");
        loadedCourse->addUnit(std::move(unit));
        QCOMPARE(loadedCourse->isModified(), true);
        loadedCourse->sync();
        QCOMPARE(loadedCourse->isModified(), false);
    }
}

void TestEditableCourseResource::skeletonUpdate()
{
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    ResourceRepositoryStub repository({language});
    auto course = EditableCourseResource::create(QUrl::fromLocalFile(":/courses/de.xml"), &repository);
    QCOMPARE(course->units().count(), 1);

    // create skeleton stub
    auto importPhrase = Phrase::create();
    importPhrase->setId("importPhraseId");
    importPhrase->setText("phraseText");
    importPhrase->setType(IPhrase::Type::Sentence);
    auto importUnit = Unit::create();
    importUnit->setId("importId");
    importUnit->addPhrase(importPhrase, importUnit->phrases().size());
    auto skeleton = CourseStub::create(language, {importUnit});

    // test import
    course->updateFrom(skeleton);
    QCOMPARE(course->units().count(), 2);
    {
        std::shared_ptr<Unit> importedUnit;
        for (auto unit : course->units()) {
            if (unit->foreignId() == importUnit->id()) {
                importedUnit = unit;
                break;
            }
        }
        QVERIFY(importedUnit != nullptr);
        QCOMPARE(importedUnit->foreignId(), importUnit->id());
        QCOMPARE(importedUnit->id(), importUnit->id());
        QCOMPARE(importedUnit->title(), importUnit->title());
        QCOMPARE(importedUnit->phrases().count(), 1);
        auto importedPhrase = importedUnit->phrases().first();
        QCOMPARE(importedPhrase->id(), importPhrase->id());
        QCOMPARE(importedPhrase->foreignId(), importPhrase->id());
        QCOMPARE(importedPhrase->text(), importPhrase->text());
        QCOMPARE(importedPhrase->type(), importPhrase->type());
    }

    // test that re-import does not change course
    course->updateFrom(skeleton);
    QCOMPARE(course->units().count(), 2);
}

QTEST_GUILESS_MAIN(TestEditableCourseResource)
