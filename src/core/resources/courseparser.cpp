/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "courseparser.h"
#include "artikulate_debug.h"
#include "core/ieditablecourse.h"
#include "core/language.h"
#include "core/phoneme.h"
#include "core/phrase.h"
#include "core/unit.h"

#include <KTar>
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QXmlStreamReader>

QXmlSchema CourseParser::loadXmlSchema(const QString &schemeName)
{
    QString relPath = QStringLiteral(":/artikulate/schemes/%1.xsd").arg(schemeName);
    QUrl file = QUrl::fromLocalFile(relPath);

    QXmlSchema schema;
    if (file.isEmpty() || schema.load(file) == false) {
        qCWarning(ARTIKULATE_PARSER()) << "Schema at file " << file.toLocalFile() << " is invalid.";
    }
    return schema;
}

QDomDocument CourseParser::loadDomDocument(const QUrl &path, const QXmlSchema &schema)
{
    QDomDocument document;
    QXmlSchemaValidator validator(schema);
    if (!validator.validate(path)) {
        qCWarning(ARTIKULATE_PARSER()) << "Schema is not valid, aborting loading of XML document:" << path.toLocalFile();
        return document;
    }

    QString errorMsg;
    QFile file(path.toLocalFile());
    if (file.open(QIODevice::ReadOnly)) {
        if (!document.setContent(&file, &errorMsg)) {
            qCWarning(ARTIKULATE_PARSER()) << errorMsg;
        }
    } else {
        qCWarning(ARTIKULATE_PARSER()) << "Could not open XML document " << path.toLocalFile() << " for reading, aborting.";
    }
    return document;
}

std::vector<std::shared_ptr<Unit>> CourseParser::parseUnits(const QUrl &path, QVector<std::shared_ptr<Phoneme>> phonemes, bool skipIncomplete)
{
    std::vector<std::shared_ptr<Unit>> units;

    QFileInfo info(path.toLocalFile());
    if (!info.exists()) {
        qCCritical(ARTIKULATE_PARSER()()) << "No course file available at location" << path.toLocalFile();
        return units;
    }

    QXmlStreamReader xml;
    QFile file(path.toLocalFile());
    if (file.open(QIODevice::ReadOnly)) {
        xml.setDevice(&file);
        xml.readNextStartElement();

        while (!xml.atEnd() && !xml.hasError()) {
            bool elementOk {false};
            QXmlStreamReader::TokenType token = xml.readNext();

            if (token == QXmlStreamReader::StartDocument) {
                continue;
            }
            if (token == QXmlStreamReader::StartElement) {
                if (xml.name() == "units") {
                    continue;
                } else if (xml.name() == "unit") {
                    auto unit = parseUnit(xml, path, phonemes, skipIncomplete, elementOk);
                    if (elementOk) {
                        units.push_back(std::move(unit));
                    }
                }
            }
        }
        if (xml.hasError()) {
            qCCritical(ARTIKULATE_PARSER()) << "Error occurred when reading Course XML file:" << path.toLocalFile();
        }
    } else {
        qCCritical(ARTIKULATE_PARSER()) << "Could not open course file" << path.toLocalFile();
    }
    xml.clear();
    file.close();

    return units;
}

std::shared_ptr<Unit> CourseParser::parseUnit(QXmlStreamReader &xml, const QUrl &path, QVector<std::shared_ptr<Phoneme>> phonemes, bool skipIncomplete, bool &ok)
{
    std::shared_ptr<Unit> unit = Unit::create();
    ok = true;

    if (xml.tokenType() != QXmlStreamReader::StartElement && xml.name() == "unit") {
        qCWarning(ARTIKULATE_PARSER()) << "Expected to parse 'unit' element, aborting here";
        return unit;
    }

    xml.readNext();
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "unit")) {
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            bool elementOk {false};
            if (xml.name() == "id") {
                unit->setId(parseElement(xml, elementOk));
                ok &= elementOk;
            } else if (xml.name() == "foreignId") {
                unit->setForeignId(parseElement(xml, elementOk));
                ok &= elementOk;
            } else if (xml.name() == "title") {
                unit->setTitle(parseElement(xml, elementOk));
                ok &= elementOk;
            } else if (xml.name() == "phrases") {
                // nothing to do
            } else if (xml.name() == "phrase") {
                auto phrase = parsePhrase(xml, path, phonemes, elementOk);
                if (elementOk && (!skipIncomplete || !phrase->soundFileUrl().isEmpty())) {
                    unit->addPhrase(phrase, unit->phrases().size());
                }
                ok &= elementOk;
            } else {
                qCWarning(ARTIKULATE_PARSER()) << "Skipping unknown token" << xml.name();
            }
        }
        xml.readNext();
    }
    if (!ok) {
        qCWarning(ARTIKULATE_PARSER()) << "Errors occurred while parsing unit" << unit->title() << unit->id();
    }
    return unit;
}

std::shared_ptr<Phrase> CourseParser::parsePhrase(QXmlStreamReader &xml, const QUrl &path, QVector<std::shared_ptr<Phoneme>> phonemes, bool &ok)
{
    std::shared_ptr<Phrase> phrase = Phrase::create();
    ok = true;

    if (xml.tokenType() != QXmlStreamReader::StartElement && xml.name() == "phrase") {
        qCWarning(ARTIKULATE_PARSER()) << "Expected to parse 'phrase' element, aborting here";
        ok = false;
        return phrase;
    }

    xml.readNext();
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "phrase")) {
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            bool elementOk {false};
            if (xml.name() == "id") {
                phrase->setId(parseElement(xml, elementOk));
                ok &= elementOk;
            } else if (xml.name() == "foreignId") {
                phrase->setForeignId(parseElement(xml, elementOk));
                ok &= elementOk;
            } else if (xml.name() == "text") {
                phrase->setText(parseElement(xml, elementOk));
                ok &= elementOk;
            } else if (xml.name() == "i18nText") {
                phrase->seti18nText(parseElement(xml, elementOk));
                ok &= elementOk;
            } else if (xml.name() == "soundFile") {
                QString fileName = parseElement(xml, elementOk);
                if (!fileName.isEmpty()) {
                    phrase->setSound(QUrl::fromLocalFile(path.adjusted(QUrl::RemoveFilename | QUrl::StripTrailingSlash).path() + '/' + fileName));
                }
                ok &= elementOk;
            } else if (xml.name() == "phonemes") {
                auto parsedPhonemeIds = parsePhonemeIds(xml, elementOk);
                for (auto phoneme : phonemes) {
                    if (parsedPhonemeIds.contains(phoneme->id())) {
                        phrase->addPhoneme(phoneme.get());
                    }
                }
                ok &= elementOk;
            } else if (xml.name() == "type") {
                const QString type = parseElement(xml, elementOk);
                if (type == "word") {
                    phrase->setType(IPhrase::Type::Word);
                } else if (type == "expression") {
                    phrase->setType(IPhrase::Type::Expression);
                } else if (type == "sentence") {
                    phrase->setType(IPhrase::Type::Sentence);
                } else if (type == "paragraph") {
                    phrase->setType(IPhrase::Type::Paragraph);
                }
                ok &= elementOk;
            } else if (xml.name() == "editState") {
                const QString type = parseElement(xml, elementOk);
                if (type == "translated") {
                    phrase->setEditState(Phrase::EditState::Translated);
                } else if (type == "completed") {
                    phrase->setEditState(Phrase::EditState::Completed);
                } else if (type == "unknown") {
                    phrase->setEditState(Phrase::EditState::Completed);
                }
                ok &= elementOk;
            } else {
                qCWarning(ARTIKULATE_PARSER()) << "Skipping unknown token" << xml.name();
            }
        }
        xml.readNext();
    }
    if (!ok) {
        qCWarning(ARTIKULATE_PARSER()) << "Errors occurred while parsing phrase" << phrase->text() << phrase->id();
    }
    return phrase;
}

QStringList CourseParser::parsePhonemeIds(QXmlStreamReader &xml, bool &ok)
{
    QStringList ids;
    ok = true;

    if (xml.tokenType() != QXmlStreamReader::StartElement && xml.name() == "phonemes") {
        qCWarning(ARTIKULATE_PARSER()) << "Expected to parse 'phonemes' element, aborting here";
        ok = false;
        return ids;
    }

    xml.readNext();
    while (!(xml.tokenType() == QXmlStreamReader::EndElement && xml.name() == "phonemes")) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement) {
            if (xml.name() == "phonemeID") {
                bool elementOk {false};
                ids.append(parseElement(xml, elementOk));
                ok &= elementOk;
            } else {
                qCWarning(ARTIKULATE_PARSER()) << "Skipping unknown token" << xml.name();
            }
        }
    }
    return ids;
}

QString CourseParser::parseElement(QXmlStreamReader &xml, bool &ok)
{
    ok = true;
    if (xml.tokenType() != QXmlStreamReader::StartElement) {
        qCCritical(ARTIKULATE_PARSER()) << "Parsing element that does not start with a start element";
        ok = false;
        return QString();
    }

    QString elementName = xml.name().toString();
    xml.readNext();

    // qCDebug(ARTIKULATE_PARSER()) << "parsed: " << elementName << " / " << xml.text().toString();
    return xml.text().toString();
}

QDomDocument CourseParser::serializedDocument(std::shared_ptr<IEditableCourse> course, bool trainingExport)
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
    for (auto unit : course->units()) {
        QDomElement unitElement = document.createElement(QStringLiteral("unit"));

        QDomElement unitIdElement = document.createElement(QStringLiteral("id"));
        QDomElement unitTitleElement = document.createElement(QStringLiteral("title"));
        QDomElement unitPhraseListElement = document.createElement(QStringLiteral("phrases"));
        unitIdElement.appendChild(document.createTextNode(unit->id()));
        unitTitleElement.appendChild(document.createTextNode(unit->title()));

        // construct phrases
        for (auto &phrase : unit->phrases()) {
            if (trainingExport && phrase->soundFileUrl().isEmpty()) {
                continue;
            }
            unitPhraseListElement.appendChild(serializedPhrase(std::static_pointer_cast<IEditablePhrase>(phrase), document));
        }

        if (trainingExport && unitPhraseListElement.childNodes().isEmpty()) {
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

QDomElement CourseParser::serializedPhrase(std::shared_ptr<IEditablePhrase> phrase, QDomDocument &document)
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
    for (auto &phoneme : phrase->phonemes()) {
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

    return phraseElement;
}

bool CourseParser::exportCourseToGhnsPackage(std::shared_ptr<IEditableCourse> course, const QString &exportPath)
{
    // filename
    const QString fileName = course->id() + ".tar.bz2";
    KTar tar = KTar(exportPath + '/' + fileName, QStringLiteral("application/x-bzip"));
    if (!tar.open(QIODevice::WriteOnly)) {
        qCWarning(ARTIKULATE_CORE()) << "Unable to open tar file" << exportPath + '/' + fileName << "in write mode, aborting.";
        return false;
    }

    for (auto &unit : course->units()) {
        for (auto &phrase : unit->phrases()) {
            if (QFile::exists(phrase->soundFileUrl())) {
                tar.addLocalFile(phrase->soundFileUrl(), phrase->id() + ".ogg");
            }
        }
    }

    tar.writeFile(course->id() + ".xml", CourseParser::serializedDocument(course, true).toByteArray());

    tar.close();
    return true;
}
