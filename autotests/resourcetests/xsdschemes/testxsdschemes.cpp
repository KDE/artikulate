/*
    SPDX-FileCopyrightText: 2013 Oindrila Gupta <oindrila.gupta92@gmail.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "testxsdschemes.h"

#include "core/xmlschema.h"
#include <QTest>
#include <QUrl>

TestXsdSchemes::TestXsdSchemes() = default;

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
    const QUrl languageFile = QUrl::fromLocalFile(":/artikulate/data/schemes/language.xsd");
    XmlSchema languageSchema;
    QVERIFY(languageSchema.load(languageFile));
    QVERIFY(languageSchema.isValid());
}

void TestXsdSchemes::courseSchemeValidationTest()
{
    const QUrl schemeFile = QUrl::fromLocalFile(":/artikulate/data/schemes/course.xsd");
    XmlSchema courseSchema;
    QVERIFY(courseSchema.load(schemeFile));
    QVERIFY(courseSchema.isValid());
}

void TestXsdSchemes::schemeValidationTest()
{
    const QUrl skeletonFile = QUrl::fromLocalFile(":/artikulate/data/schemes/skeleton.xsd");
    XmlSchema skeletonScheme;
    QVERIFY(skeletonScheme.load(skeletonFile));
    QVERIFY(skeletonScheme.isValid());
}

QTEST_GUILESS_MAIN(TestXsdSchemes)
