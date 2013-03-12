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

#include <qtest_kde.h>
#include <KDebug>
#include <KTemporaryFile>
#include <KStandardDirs>

#include <QIODevice>
#include <QFile>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QDomDocument>

TestCourseFiles::TestCourseFiles()
{
    KGlobal::dirs()->addResourceDir("appdata" , "./");
}

void TestCourseFiles::init()
{
    // TODO initialization of test case
}

void TestCourseFiles::cleanup()
{
    // TODO cleanup after test run
}

void TestCourseFiles::courseSchemeValidationTest()
{
    KUrl schemeFile = KUrl::fromLocalFile("schemes/course.xsd");
    QXmlSchema courseSchema;
    QVERIFY(courseSchema.load(schemeFile));
    QVERIFY(courseSchema.isValid());

    KUrl languageFile = KUrl::fromLocalFile("schemes/language.xsd");
    QXmlSchema languageSchema;
    QVERIFY(languageSchema.load(languageFile));
    QVERIFY(languageSchema.isValid());
}

void TestCourseFiles::fileLoadSaveCompleteness()
{
    ResourceManager manager;
    manager.loadLanguage(KUrl::fromLocalFile("data/languages/de.xml"));
    manager.addCourse(KUrl::fromLocalFile("data/courses/de.xml"));

    // test to encure further logic
    QVERIFY(manager.courseList().count() == 1);

    Course *testCourse = manager.courseList().first();
    KTemporaryFile outputFile;
    outputFile.setSuffix(".xml");
    outputFile.open();
    KUrl oldFileName = testCourse->file();
    testCourse->setFile(KUrl::fromLocalFile(outputFile.fileName()));
    testCourse->sync();
    testCourse->setFile(oldFileName); // restore for later tests

    QFile file(outputFile.fileName());
    if (!file.open(QIODevice::ReadOnly)) {
        kFatal() << "Could not open file to read.";
    }

    //TODO this only works, since the resource manager not checks uniqueness of course ids!
    manager.addCourse(KUrl::fromLocalFile(outputFile.fileName()));
    Course *compareCourse = manager.courseList().last();

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
    QVERIFY(testUnit->phraseList(Phrase::AllTypes).count() == compareUnit->phraseList(Phrase::AllTypes).count());

    Phrase *testPhrase = testUnit->phraseList(Phrase::AllTypes).first();
    Phrase *comparePhrase;
    // Note that this actually means that we DO NOT respect phrase orders by list order!
    foreach (Phrase *phrase, compareUnit->phraseList(Phrase::AllTypes)) {
        if (testPhrase->id() == phrase->id()) {
            comparePhrase = phrase;
            break;
        }
    }
    QVERIFY(testPhrase->id() == comparePhrase->id());
    QVERIFY(testPhrase->foreignId() == comparePhrase->foreignId());
    QVERIFY(testPhrase->text() == comparePhrase->text());
    QVERIFY(testPhrase->type() == comparePhrase->type());
    QVERIFY(testPhrase->sound().toLocalFile() == comparePhrase->sound().toLocalFile());
    QVERIFY(testPhrase->phonemes().count() == comparePhrase->phonemes().count());
    //FIXME implement phoneme checks after phonemes are fully implemented
}


QTEST_KDEMAIN_CORE(TestCourseFiles)
