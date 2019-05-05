/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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
#include "resourcerepositorystub.h"
#include "core/language.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "core/resources/courseparser.h"
#include "core/resources/languageresource.h"
#include "core/resources/editablecourseresource.h"

#include <QTest>
#include <QDebug>
#include <QTemporaryFile>
#include <QSignalSpy>
#include <QIODevice>
#include <QFile>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QDomDocument>

TestEditableCourseResource::TestEditableCourseResource()
{
}

void TestEditableCourseResource::init()
{
}

void TestEditableCourseResource::cleanup()
{
}

void TestEditableCourseResource::courseSchemeValidationTest()
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

void TestEditableCourseResource::loadCourseResource()
{
    Language language;
    language.setId("de");
    ResourceRepositoryStub repository({&language});

    EditableCourseResource course(QUrl::fromLocalFile(":/courses/de.xml"), &repository);
    QCOMPARE(course.file().toLocalFile(), ":/courses/de.xml");
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
    QCOMPARE(firstPhrase->soundFileUrl(), ":/courses/de_01.ogg");
    QCOMPARE(firstPhrase->type(), Phrase::Type::Sentence);
    QVERIFY(firstPhrase->phonemes().isEmpty());
}

void TestEditableCourseResource::unitAddAndRemoveHandling()
{
    // boilerplate
    Language language;
    language.setId("de");
    ResourceRepositoryStub repository({&language});
    EditableCourseResource course(QUrl::fromLocalFile(":/courses/de.xml"), &repository);

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

void TestEditableCourseResource::coursePropertyChanges()
{
    // boilerplate
    Language language;
    language.setId("de");
    ResourceRepositoryStub repository({&language});
    CourseResource course(QUrl::fromLocalFile(":/courses/de.xml"), &repository);

    // id
    {
        const QString value = "newId";
        QSignalSpy spy(&course, SIGNAL(idChanged()));
        QCOMPARE(spy.count(), 0);
        course.setId(value);
        QCOMPARE(course.id(), value);
        QCOMPARE(spy.count(), 1);
    }

    // foreign id
    {
        const QString value = "newForeignId";
        QSignalSpy spy(&course, SIGNAL(foreignIdChanged()));
        QCOMPARE(spy.count(), 0);
        course.setForeignId(value);
        QCOMPARE(course.foreignId(), value);
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

    // title
    {
        const QString value = "newI18nTitle";
        QSignalSpy spy(&course, SIGNAL(i18nTitleChanged()));
        QCOMPARE(spy.count(), 0);
        course.setI18nTitle(value);
        QCOMPARE(course.i18nTitle(), value);
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

    // language
    {
        Language testLanguage;
        QSignalSpy spy(&course, SIGNAL(languageChanged()));
        QCOMPARE(spy.count(), 0);
        course.setLanguage(&testLanguage);
        QCOMPARE(course.language(), &testLanguage);
        QCOMPARE(spy.count(), 1);
    }
}

void TestEditableCourseResource::fileLoadSaveCompleteness()
{
    // boilerplate
    Language language;
    language.setId("de");
    ResourceRepositoryStub repository({&language});
    EditableCourseResource course(QUrl::fromLocalFile(":/courses/de.xml"), &repository);

    QTemporaryFile outputFile;
    outputFile.open();
    course.exportCourse(QUrl::fromLocalFile(outputFile.fileName()));

    // note: this only works, since the resource manager not checks uniqueness of course ids!
    EditableCourseResource loadedCourse(QUrl::fromLocalFile(outputFile.fileName()), &repository);

    // test that we actually call the different files
    QVERIFY(course.file().toLocalFile() != loadedCourse.file().toLocalFile());
    QVERIFY(course.id() == loadedCourse.id());
    QVERIFY(course.foreignId() == loadedCourse.foreignId());
    QVERIFY(course.title() == loadedCourse.title());
    QVERIFY(course.description() == loadedCourse.description());
    QVERIFY(course.language()->id() == loadedCourse.language()->id());
    QVERIFY(course.unitList().count() == loadedCourse.unitList().count());

    Unit *testUnit = course.unitList().constFirst();
    Unit *compareUnit = loadedCourse.unitList().constFirst();
    QVERIFY(testUnit->id() == compareUnit->id());
    QVERIFY(testUnit->foreignId() == compareUnit->foreignId());
    QVERIFY(testUnit->title() == compareUnit->title());
    QVERIFY(testUnit->phraseList().count() == compareUnit->phraseList().count());

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

QTEST_GUILESS_MAIN(TestEditableCourseResource)
