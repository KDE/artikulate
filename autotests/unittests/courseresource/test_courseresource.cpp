/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "test_courseresource.h"
#include "../mocks/languagestub.h"
#include "core/language.h"
#include "core/phonemegroup.h"
#include "core/phrase.h"
#include "core/resources/courseresource.h"
#include "core/unit.h"
#include "resourcerepositorystub.h"
#include <QSignalSpy>
#include <QTest>

TestCourseResource::TestCourseResource()
{
}

void TestCourseResource::init()
{
}

void TestCourseResource::cleanup()
{
}

void TestCourseResource::loadCourseResource()
{
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    auto group = std::static_pointer_cast<LanguageStub>(language)->addPhonemeGroup("id", "title");
    group->addPhoneme("g", "G");
    group->addPhoneme("u", "U");
    std::vector<std::shared_ptr<ILanguage>> languages;
    languages.push_back(language);
    ResourceRepositoryStub repository(languages);

    const QString courseDirectory = qApp->applicationDirPath() + "/../autotests/unittests/data/courses/de/";
    const QString courseFile = courseDirectory + "de.xml";

    auto course = CourseResource::create(QUrl::fromLocalFile(courseFile), &repository);
    QCOMPARE(course->file().toLocalFile(), courseFile);
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
    QCOMPARE(firstPhrase->soundFileUrl(), courseDirectory + "de_01.ogg");
    QCOMPARE(firstPhrase->type(), Phrase::Type::Sentence);
    QCOMPARE(firstPhrase->phonemes().count(), 2);
}

void TestCourseResource::loadCourseResourceSkipIncomplete()
{
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    auto group = std::static_pointer_cast<LanguageStub>(language)->addPhonemeGroup("id", "title");
    group->addPhoneme("g", "G");
    group->addPhoneme("u", "U");
    std::vector<std::shared_ptr<ILanguage>> languages;
    languages.push_back(language);
    ResourceRepositoryStub repository(languages);

    const QString courseDirectory = "data/courses/de/";
    const QString courseFile = courseDirectory + "de.xml";

    auto course = CourseResource::create(QUrl::fromLocalFile(courseFile), &repository, true);
    QCOMPARE(course->file().toLocalFile(), courseFile);
    QCOMPARE(course->id(), "de");
    QCOMPARE(course->units().count(), 1);
    QCOMPARE(course->units().first()->course(), course);

    const auto unit = course->units().first();
    QVERIFY(unit != nullptr);
    QCOMPARE(unit->id(), "1");
    QCOMPARE(unit->phrases().count(), 2);
}

void TestCourseResource::unitAddAndRemoveHandling()
{
    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    ResourceRepositoryStub repository({language});

    const QString courseDirectory = qApp->applicationDirPath() + "/../autotests/unittests/data/courses/de/";
    const QString courseFile = courseDirectory + "de.xml";
    auto course = CourseResource::create(QUrl::fromLocalFile(courseFile), &repository);

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

void TestCourseResource::coursePropertyChanges()
{
    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    ResourceRepositoryStub repository({language});

    const QString courseDirectory = qApp->applicationDirPath() + "/../autotests/unittests/data/courses/de/";
    const QString courseFile = courseDirectory + "de.xml";
    auto course = CourseResource::create(QUrl::fromLocalFile(courseFile), &repository);

    // id
    {
        const QString value = "newId";
        QSignalSpy spy(course.get(), SIGNAL(idChanged()));
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

QTEST_GUILESS_MAIN(TestCourseResource)
