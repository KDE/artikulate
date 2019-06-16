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

#include "testlanguagefiles.h"
#include "core/language.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "core/resources/courseparser.h"

#include <QTest>
#include <QDebug>
#include <QTemporaryFile>
#include <QDirIterator>
#include <QUrl>

#include <QIODevice>
#include <QFile>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QDomDocument>
#include <QStandardPaths>

TestLanguageFiles::TestLanguageFiles()
{
}

void TestLanguageFiles::init()
{
    // TODO initialization of test case
}

void TestLanguageFiles::cleanup()
{
    // TODO cleanup after test run
}

void TestLanguageFiles::languageSchemeValidationTest()
{
    QUrl languageFile = QUrl::fromLocalFile(":/artikulate/schemes/language.xsd");
    QXmlSchema languageSchema;
    QVERIFY(languageSchema.load(languageFile));
    QVERIFY(languageSchema.isValid());
}

void TestLanguageFiles::checkIdUniqueness()
{
    QDirIterator iter(QDir(":/artikulate/languages/"));
    while (iter.hasNext()) {
        const QString &file = iter.next();
        qDebug() << "File being parsed: " << file;
        QStringList idList;
        const QUrl &languageFile = QUrl::fromLocalFile(file);
        QVERIFY(languageFile.isLocalFile());

        QXmlSchema schema = CourseParser::loadXmlSchema(QStringLiteral("language"));
        QVERIFY(schema.isValid());

        QDomDocument document = CourseParser::loadDomDocument(languageFile, schema);
        QVERIFY(!document.isNull());

        QDomElement root(document.documentElement());
        std::unique_ptr<Language> language(new Language);
        language->setFile(languageFile);
        language->setId(root.firstChildElement(QStringLiteral("id")).text());
        language->setTitle(root.firstChildElement(QStringLiteral("title")).text());
        // create phoneme groups
        for (QDomElement groupNode = root.firstChildElement(QStringLiteral("phonemeGroups")).firstChildElement();
             !groupNode.isNull();
             groupNode = groupNode.nextSiblingElement())
        {
            for (QDomElement phonemeNode = groupNode.firstChildElement(QStringLiteral("phonemes")).firstChildElement();
                 !phonemeNode.isNull();
                 phonemeNode = phonemeNode.nextSiblingElement())
            {
                QString id = phonemeNode.firstChildElement(QStringLiteral("id")).text();
                qDebug() << "ID: " << id;
                QVERIFY2(!idList.contains(id),"Phoneme ID used more than once in the tested file");
                idList.append(id);
            }
        }
    }
}

QTEST_GUILESS_MAIN(TestLanguageFiles)
