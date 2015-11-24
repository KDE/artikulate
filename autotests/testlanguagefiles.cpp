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
#include "core/resourcemanager.h"
#include "core/course.h"
#include "core/language.h"
#include "core/unit.h"
#include "core/phrase.h"

#include <QTest>
#include <QDebug>
#include <QTemporaryFile>
#include <QUrl>

#include <QIODevice>
#include <QFile>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QDomDocument>
#include <QStandardPaths>

TestLanguageFiles::TestLanguageFiles()
{
    //FIXME port this
//     KGlobal::dirs()->addResourceDir("appdata" , "./autotests/data");
//     KGlobal::dirs()->addResourceDir("appdata" , "./autotests");
//     KGlobal::dirs()->addResourceDir("appdata" , "./");
}

void TestLanguageFiles::init()
{
    // TODO initialization of test case
}

void TestLanguageFiles::cleanup()
{
    // TODO cleanup after test run
}

QXmlSchema TestLanguageFiles::loadXmlSchema(const QString &schemeName) const
{
    QString relPath = QString("schemes/%1.xsd").arg(schemeName);
    QUrl file = QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::DataLocation, relPath));

    QXmlSchema schema;
    if (schema.load(file) == false) {
        qWarning() << "Schema at file " << file.toLocalFile() << " is invalid.";
    }
    return schema;
}

QDomDocument TestLanguageFiles::loadDomDocument(const QUrl &path, const QXmlSchema &schema) const
{
    QDomDocument document;
    QXmlSchemaValidator validator(schema);
    if (!validator.validate(path)) {
        qWarning() << "Schema is not valid, aborting loading of XML document:" << path.toLocalFile();
        return document;
    }

    QString errorMsg;
    QFile file(path.toLocalFile());
    if (file.open(QIODevice::ReadOnly)) {
        if (!document.setContent(&file, &errorMsg)) {
            qWarning() << errorMsg;
        }
    } else {
        qWarning() << "Could not open XML document " << path.toLocalFile() << " for reading, aborting.";
    }
    return document;
}

void TestLanguageFiles::languageSchemeValidationTest()
{
    QUrl languageFile = QUrl::fromLocalFile("schemes/language.xsd");
    QXmlSchema languageSchema;
    QVERIFY(languageSchema.load(languageFile));
    QVERIFY(languageSchema.isValid());
}

void TestLanguageFiles::checkIdUniqueness()
{
    ResourceManager manager;
    QStringList languageFiles = QStandardPaths::locateAll(QStandardPaths::DataLocation, QString("data/languages/*.xml"));
    foreach (const QString &file, languageFiles) {
        qDebug() << "File being parsed: " << file;
        QStringList idList;
        const QUrl &languageFile = QUrl::fromLocalFile(file);
        QVERIFY(languageFile.isLocalFile());

        QXmlSchema schema = loadXmlSchema("language");
        QVERIFY(schema.isValid());

        QDomDocument document = loadDomDocument(languageFile, schema);
        QVERIFY(!document.isNull());

        QDomElement root(document.documentElement());
        Language *language = new Language(this);
        language->setFile(languageFile);
        language->setId(root.firstChildElement("id").text());
        language->setTitle(root.firstChildElement("title").text());
        // create phoneme groups
        for (QDomElement groupNode = root.firstChildElement("phonemeGroups").firstChildElement();
             !groupNode.isNull();
             groupNode = groupNode.nextSiblingElement())
        {
            for (QDomElement phonemeNode = groupNode.firstChildElement("phonemes").firstChildElement();
                 !phonemeNode.isNull();
                 phonemeNode = phonemeNode.nextSiblingElement())
            {
                QString id = phonemeNode.firstChildElement("id").text();
                qDebug() << "ID: " << id;
                QVERIFY2(!idList.contains(id),"Phoneme ID used more than once in the tested file");
                idList.append(id);
            }
        }
    }
}


QTEST_GUILESS_MAIN(TestLanguageFiles)
