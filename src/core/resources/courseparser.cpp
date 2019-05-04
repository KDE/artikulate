/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "courseparser.h"
#include "core/icourse.h"
#include "core/language.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "core/phoneme.h"
#include "artikulate_debug.h"

#include <QDomDocument>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <KTar>


QXmlSchema CourseParser::loadXmlSchema(const QString &schemeName)
{
    QString relPath = QStringLiteral(":/artikulate/schemes/%1.xsd").arg(schemeName);
    QUrl file = QUrl::fromLocalFile(relPath);

    QXmlSchema schema;
    if (file.isEmpty() || schema.load(file) == false) {
        qCWarning(ARTIKULATE_LOG) << "Schema at file " << file.toLocalFile() << " is invalid.";
    }
    return schema;
}

QDomDocument CourseParser::loadDomDocument(const QUrl &path, const QXmlSchema &schema)
{
    QDomDocument document;
    QXmlSchemaValidator validator(schema);
    if (!validator.validate(path)) {
        qCWarning(ARTIKULATE_LOG) << "Schema is not valid, aborting loading of XML document:" << path.toLocalFile();
        return document;
    }

    QString errorMsg;
    QFile file(path.toLocalFile());
    if (file.open(QIODevice::ReadOnly)) {
        if (!document.setContent(&file, &errorMsg)) {
            qCWarning(ARTIKULATE_LOG) << errorMsg;
        }
    } else {
        qCWarning(ARTIKULATE_LOG) << "Could not open XML document " << path.toLocalFile() << " for reading, aborting.";
    }
    return document;
}

Phrase * CourseParser::parsePhrase(QDomElement phraseNode, Unit* parentUnit)
{
    const ICourse *course = parentUnit->course();
    Q_ASSERT(course != nullptr);

    Phrase *phrase = new Phrase(parentUnit);
    phrase->setId(phraseNode.firstChildElement(QStringLiteral("id")).text());
    phrase->setText(phraseNode.firstChildElement(QStringLiteral("text")).text());
    phrase->seti18nText(phraseNode.firstChildElement(QStringLiteral("i18nText")).text());
    phrase->setUnit(parentUnit);
    if (!phraseNode.firstChildElement(QStringLiteral("soundFile")).text().isEmpty()) {
        phrase->setSound(QUrl::fromLocalFile(
                course->file().adjusted(QUrl::RemoveFilename|QUrl::StripTrailingSlash).path()
                + '/' + phraseNode.firstChildElement(QStringLiteral("soundFile")).text())
            );
    }
    phrase->setType(phraseNode.firstChildElement(QStringLiteral("type")).text());
    phrase->setEditState(phraseNode.firstChildElement(QStringLiteral("editState")).text());
    if (!phraseNode.firstChildElement(QStringLiteral("foreignId")).isNull()) {
        phrase->setForeignId(phraseNode.firstChildElement(QStringLiteral("foreignId")).text());
    }

    // add phonemes
    QList<Phoneme *> phonemes = course->language()->phonemes();
    for (QDomElement phonemeID = phraseNode.firstChildElement(QStringLiteral("phonemes")).firstChildElement();
        !phonemeID.isNull();
            phonemeID = phonemeID.nextSiblingElement())
    {
        QString id = phonemeID.text();
        if (id.isEmpty()) {
            qCritical() << "Phoneme ID string is empty for phrase "<< phrase->id() <<", aborting.";
            continue;
        }
        for (Phoneme *phoneme : phonemes) {
            if (phoneme->id() == id) {
                phrase->addPhoneme(phoneme);
                break;
            }
        }
    }

    if (!phraseNode.firstChildElement(QStringLiteral("excluded")).isNull() &&
        phraseNode.firstChildElement(QStringLiteral("excluded")).text() == QLatin1String("true"))
    {
        phrase->setExcluded(true);
    }

    return phrase;
}

QDomDocument CourseParser::serializedDocument(ICourse *course, bool trainingExport)
{
    QDomDocument document;
    // prepare xml header
    QDomProcessingInstruction header = document.createProcessingInstruction(QStringLiteral("xml"), QStringLiteral("version=\"1.0\""));
    document.appendChild(header);

    // create main element
    QDomElement root = document.createElement(QStringLiteral("course"));
    document.appendChild(root);

    QDomElement idElement = document.createElement(QStringLiteral("id"));
    QDomElement titleElement = document.createElement(QStringLiteral("title"));
    QDomElement descriptionElement = document.createElement(QStringLiteral("description"));
    QDomElement languageElement = document.createElement(QStringLiteral("language"));

    idElement.appendChild(document.createTextNode(course->id()));
    titleElement.appendChild(document.createTextNode(course->title()));
    descriptionElement.appendChild(document.createTextNode(course->description()));
    languageElement.appendChild(document.createTextNode(course->id()));

    QDomElement unitListElement = document.createElement(QStringLiteral("units"));
    // create units
    for (Unit *unit : course->unitList()) {
        QDomElement unitElement = document.createElement(QStringLiteral("unit"));

        QDomElement unitIdElement = document.createElement(QStringLiteral("id"));
        QDomElement unitTitleElement = document.createElement(QStringLiteral("title"));
        QDomElement unitPhraseListElement = document.createElement(QStringLiteral("phrases"));
        unitIdElement.appendChild(document.createTextNode(unit->id()));
        unitTitleElement.appendChild(document.createTextNode(unit->title()));

        // construct phrases
        for (Phrase *phrase : unit->phraseList()) {
            if (trainingExport && phrase->soundFileUrl().isEmpty()) {
                continue;
            }
            unitPhraseListElement.appendChild(serializedPhrase(phrase, document));
        }

        if (trainingExport && unitPhraseListElement.childNodes().count() == 0) {
            continue;
        }

        // construct the unit element
        unitElement.appendChild(unitIdElement);
        if (!unit->foreignId().isEmpty()) {
            QDomElement unitForeignIdElement = document.createElement(QStringLiteral("foreignId"));
            unitForeignIdElement.appendChild(document.createTextNode(unit->foreignId()));
            unitElement.appendChild(unitForeignIdElement);
        }
        unitElement.appendChild(unitTitleElement);
        unitElement.appendChild(unitPhraseListElement);

        unitListElement.appendChild(unitElement);
    }

    root.appendChild(idElement);
    if (!course->foreignId().isEmpty()) {
        QDomElement courseForeignIdElement = document.createElement(QStringLiteral("foreignId"));
        courseForeignIdElement.appendChild(document.createTextNode(course->foreignId()));
        root.appendChild(courseForeignIdElement);
    }
    root.appendChild(titleElement);
    root.appendChild(descriptionElement);
    root.appendChild(languageElement);
    root.appendChild(unitListElement);

    return document;
}

QDomElement CourseParser::serializedPhrase(Phrase *phrase, QDomDocument &document)
{
    QDomElement phraseElement = document.createElement(QStringLiteral("phrase"));
    QDomElement phraseIdElement = document.createElement(QStringLiteral("id"));
    QDomElement phraseTextElement = document.createElement(QStringLiteral("text"));
    QDomElement phrasei18nTextElement = document.createElement(QStringLiteral("i18nText"));
    QDomElement phraseSoundFileElement = document.createElement(QStringLiteral("soundFile"));
    QDomElement phraseTypeElement = document.createElement(QStringLiteral("type"));
    QDomElement phraseEditStateElement = document.createElement(QStringLiteral("editState"));
    QDomElement phrasePhonemeListElement = document.createElement(QStringLiteral("phonemes"));

    phraseIdElement.appendChild(document.createTextNode(phrase->id()));
    phraseTextElement.appendChild(document.createTextNode(phrase->text()));
    phrasei18nTextElement.appendChild(document.createTextNode(phrase->i18nText()));
    phraseSoundFileElement.appendChild(document.createTextNode(phrase->sound().fileName()));
    phraseTypeElement.appendChild(document.createTextNode(phrase->typeString()));
    phraseEditStateElement.appendChild(document.createTextNode(phrase->editStateString()));

    // add phonemes
    foreach (Phoneme *phoneme, phrase->phonemes()) {
        QDomElement phonemeElement = document.createElement(QStringLiteral("phonemeID"));
        phonemeElement.appendChild(document.createTextNode(phoneme->id()));
        phrasePhonemeListElement.appendChild(phonemeElement);
    }

    phraseElement.appendChild(phraseIdElement);
    if (!phrase->foreignId().isEmpty()) {
        QDomElement phraseForeignIdElement = document.createElement(QStringLiteral("foreignId"));
        phraseForeignIdElement.appendChild(document.createTextNode(phrase->foreignId()));
        phraseElement.appendChild(phraseForeignIdElement);
    }
    phraseElement.appendChild(phraseTextElement);
    phraseElement.appendChild(phrasei18nTextElement);
    phraseElement.appendChild(phraseSoundFileElement);
    phraseElement.appendChild(phraseTypeElement);
    phraseElement.appendChild(phraseEditStateElement);
    phraseElement.appendChild(phrasePhonemeListElement);

    if (phrase->isExcluded()) {
        QDomElement phraseIsExcludedElement = document.createElement(QStringLiteral("excluded"));
        phraseIsExcludedElement.appendChild(document.createTextNode(QStringLiteral("true")));
        phraseElement.appendChild(phraseIsExcludedElement);
    }

    return phraseElement;
}

bool CourseParser::exportCourseToGhnsPackage(ICourse *course, const QString &exportPath)
{
    // filename
    const QString fileName = course->id() + ".tar.bz2";
    KTar tar = KTar(exportPath + '/' + fileName, QStringLiteral("application/x-bzip"));
    if (!tar.open(QIODevice::WriteOnly)) {
        qCWarning(ARTIKULATE_CORE()) << "Unable to open tar file"
            << exportPath + '/' + fileName
            << "in write mode, aborting.";
        return false;
    }

    for (auto *unit : course->unitList()) {
        for (auto *phrase : unit->phraseList()) {
            if (QFile::exists(phrase->soundFileUrl())) {
                tar.addLocalFile(phrase->soundFileUrl(), phrase->id() + ".ogg");
            }
        }
    }

    tar.writeFile(course->id() + ".xml", CourseParser::serializedDocument(course, true).toByteArray());

    tar.close();
    return true;
}
