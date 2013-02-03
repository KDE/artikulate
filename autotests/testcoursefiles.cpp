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

#include <qtest_kde.h>
#include <KDebug>

#include <QIODevice>
#include <QFile>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QDomDocument>

TestCourseFiles::TestCourseFiles()
{
    // TODO prepare
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


QTEST_KDEMAIN_CORE(TestCourseFiles)
