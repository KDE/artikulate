/*
 *  Copyright 2013  Oindrila Gupta <oindrila.gupta92@gmail.com>
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

#include "testxsdschemes.h"

#include <QTest>
#include <QDebug>
#include <QUrl>
#include <QXmlSchema>
#include <QXmlSchemaValidator>

TestXsdSchemes::TestXsdSchemes()
{
}

void TestXsdSchemes::init()
{
    // TODO initialization of test case
}

void TestXsdSchemes::cleanup()
{
    // TODO cleanup after test run
}

void TestXsdSchemes::languageSchemeValidationTest()
{
    QUrl languageFile = QUrl::fromLocalFile(":/artikulate/schemes/language.xsd");
    QXmlSchema languageSchema;
    QVERIFY(languageSchema.load(languageFile));
    QVERIFY(languageSchema.isValid());
}

void TestXsdSchemes::courseSchemeValidationTest()
{
    QUrl schemeFile = QUrl::fromLocalFile(":/artikulate/schemes/course.xsd");
    QXmlSchema courseSchema;
    QVERIFY(courseSchema.load(schemeFile));
    QVERIFY(courseSchema.isValid());
}

void TestXsdSchemes::schemeValidationTest()
{
    QUrl skeletonFile = QUrl::fromLocalFile(":/artikulate/schemes/skeleton.xsd");
    QXmlSchema skeletonScheme;
    QVERIFY(skeletonScheme.load(skeletonFile));
    QVERIFY(skeletonScheme.isValid());
}

QTEST_GUILESS_MAIN(TestXsdSchemes)
