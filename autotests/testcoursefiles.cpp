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

#include "testcoursefiles.h"
#include "core/resourcemanager.h"
#include "core/course.h"
#include "core/language.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "core/resources/languageresource.h"
#include "core/resources/courseresource.h"
#include "../src/settings.h"

#include <QTest>
#include <QDebug>
#include <QTemporaryFile>

#include <QIODevice>
#include <QFile>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QDomDocument>

TestCourseFiles::TestCourseFiles()
    : m_systemUseCourseRepositoryValue(Settings::useCourseRepository())
{

}

void TestCourseFiles::init()
{
    //FIXME has to be ported
//     KGlobal::dirs()->addResourceDir("appdata" , "./testcourses/");
//     KGlobal::dirs()->addResourceDir("appdata" , "./");
//     KGlobal::dirs()->addResourceDir("appdata" , "./autotests/");
//     KGlobal::dirs()->addResourceDir("appdata" , "./autotests/testcourses/");

    Settings::setUseCourseRepository(false);
    Settings::self()->save();
}

void TestCourseFiles::cleanup()
{
    // reset value
    Settings::setUseCourseRepository(m_systemUseCourseRepositoryValue);
    Settings::self()->save();
}

void TestCourseFiles::courseSchemeValidationTest()
{
    QUrl schemeFile = QUrl::fromLocalFile("schemes/course.xsd");
    QXmlSchema courseSchema;
    QVERIFY(courseSchema.load(schemeFile));
    QVERIFY(courseSchema.isValid());

    QUrl skeletonFile = QUrl::fromLocalFile("schemes/skeleton.xsd");
    QXmlSchema skeletonScheme;
    QVERIFY(skeletonScheme.load(skeletonFile));
    QVERIFY(skeletonScheme.isValid());
}

void TestCourseFiles::fileLoadSaveCompleteness()
{
    ResourceManager manager;
    manager.addLanguage(QUrl::fromLocalFile("data/languages/de.xml"));
    manager.addCourse(QUrl::fromLocalFile("data/courses/de.xml"));

    // test to encure further logic
    QVERIFY(manager.courseResources(manager.languageResources().first()->language()).count() == 1);

    Course *testCourse = manager.courseResources(manager.languageResources().first()->language()).first()->course();
    QTemporaryFile outputFile;
    outputFile.open();
    QUrl oldFileName = testCourse->file();
    testCourse->setFile(QUrl::fromLocalFile(outputFile.fileName()));
    testCourse->setLanguage(manager.languageResources().first()->language());
    testCourse->sync();
    testCourse->setFile(oldFileName); // restore for later tests

    QFile file(outputFile.fileName());
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "Could not open file to read.";
    }

    //TODO this only works, since the resource manager not checks uniqueness of course ids!
    manager.addCourse(QUrl::fromLocalFile(outputFile.fileName()));
    Course *compareCourse = manager.courseResources(manager.languageResources().first()->language()).last()->course();

    // test that we actually call the different files
    QVERIFY(testCourse->file().toLocalFile() != compareCourse->file().toLocalFile());
    QVERIFY(testCourse->id() == compareCourse->id());
    QVERIFY(testCourse->foreignId() == compareCourse->foreignId());
    QVERIFY(testCourse->title() == compareCourse->title());
    QVERIFY(testCourse->description() == compareCourse->description());
    QVERIFY(testCourse->language()->id() == compareCourse->language()->id());
    QVERIFY(testCourse->unitList().count() == compareCourse->unitList().count());

    Unit *testUnit = testCourse->unitList().first();
    Unit *compareUnit = compareCourse->unitList().first();
    QVERIFY(testUnit->id() == compareUnit->id());
    QVERIFY(testUnit->foreignId() == compareUnit->foreignId());
    QVERIFY(testUnit->title() == compareUnit->title());
    QVERIFY(testUnit->phraseList().count() == compareUnit->phraseList().count());

    Phrase *testPhrase = testUnit->phraseList().first();
    Phrase *comparePhrase = new Phrase(this);
    // Note that this actually means that we DO NOT respect phrase orders by list order!
    foreach (Phrase *phrase, compareUnit->phraseList()) {
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
    //FIXME implement phoneme checks after phonemes are fully implemented
}


QTEST_GUILESS_MAIN(TestCourseFiles)
