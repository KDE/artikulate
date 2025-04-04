/*
    SPDX-FileCopyrightText: 2013 Oindrila Gupta <oindrila.gupta92@gmail.com>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "testxsdschemes.h"

#include <QTest>
#include <QUrl>
#include "core/xmlschema.h"

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
    XmlSchema languageSchema;
    QVERIFY(languageSchema.load(languageFile));
    QVERIFY(languageSchema.isValid());
}

void TestXsdSchemes::courseSchemeValidationTest()
{
    QUrl schemeFile = QUrl::fromLocalFile(":/artikulate/schemes/course.xsd");
    XmlSchema courseSchema;
    QVERIFY(courseSchema.load(schemeFile));
    QVERIFY(courseSchema.isValid());
}

void TestXsdSchemes::schemeValidationTest()
{
    QUrl skeletonFile = QUrl::fromLocalFile(":/artikulate/schemes/skeleton.xsd");
    XmlSchema skeletonScheme;
    QVERIFY(skeletonScheme.load(skeletonFile));
    QVERIFY(skeletonScheme.isValid());
}

QTEST_GUILESS_MAIN(TestXsdSchemes)
