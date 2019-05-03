/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *  Copyright 2013       Oindrila Gupta <oindrila.gupta92@gmail.com>
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

#include "courseresource.h"
#include "core/resourcemanager.h"
#include "core/language.h"
#include "core/course.h"
#include "core/unit.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"
#include "core/language.h"
#include "core/iresourcerepository.h"
#include "application.h"

#include <QXmlSchema>
#include <QXmlStreamReader>
#include <QXmlSchemaValidator>
#include <QStandardPaths>
#include <QDomDocument>
#include <QIODevice>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <KTar>

#include "artikulate_debug.h"

class CourseResourcePrivate
{
public:
    CourseResourcePrivate() = default;

    void loadCourse();
    Phrase * parsePhrase(QDomElement phraseNode, Unit* parentUnit) const;

    /**
     * Load XSD file given by its file name (without ".xsd" suffix). The method searches exclusively
     * the standard install dir for XSD files in subdirectory "schemes/".
     *
     * \param schemeName name of the Xml schema without suffix
     * \return loaded XML Schema
     */
    QXmlSchema loadXmlSchema(const QString &schemeName) const;

    /**
     * Load XML file given by \p file that confirms with XML schema \p scheme.
     *
     * \param path is the path to the XML file to be loaded
     * \param scheme is the XML schema describing the DOM
     * \return the loaded DOM document
     */
    QDomDocument loadDomDocument(const QUrl &path, const QXmlSchema &schema) const;

    IResourceRepository *m_repository{ nullptr };
    QUrl m_path;
    QString m_identifier;
    QString m_title;
    QString m_languageId;
    Language *m_language{ nullptr };
    QString m_i18nTitle;
    QString m_description;
    Course *m_loadedCourse{ nullptr };
};

QXmlSchema CourseResourcePrivate::loadXmlSchema(const QString &schemeName) const
{
    QString relPath = QStringLiteral(":/artikulate/schemes/%1.xsd").arg(schemeName);
    QUrl file = QUrl::fromLocalFile(relPath);

    QXmlSchema schema;
    if (file.isEmpty() || schema.load(file) == false) {
        qCWarning(ARTIKULATE_LOG) << "Schema at file " << file.toLocalFile() << " is invalid.";
    }
    return schema;
}

QDomDocument CourseResourcePrivate::loadDomDocument(const QUrl &path, const QXmlSchema &schema) const
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


CourseResource::CourseResource(const QUrl &path, IResourceRepository *repository)
    : ICourse(repository)
    , d(new CourseResourcePrivate())
{
    d->m_path = path;
    d->m_repository = repository;

    // load basic information from language file, but does not parse everything
    QXmlStreamReader xml;
    QFile file(path.toLocalFile());
    if (file.open(QIODevice::ReadOnly)) {
        xml.setDevice(&file);
        xml.readNextStartElement();
        while (xml.readNext() && !xml.atEnd()) {
            if (xml.name() == "id") {
                d->m_identifier = xml.readElementText();
                continue;
            }
            //TODO i18nTitle must be implemented, currently missing and hence not parsed
            if (xml.name() == "title") {
                d->m_title = xml.readElementText();
                d->m_i18nTitle = d->m_title;
                continue;
            }
            if (xml.name() == "description") {
                d->m_description = xml.readElementText();
                continue;
            }
            if (xml.name() == "language") {
                d->m_languageId = xml.readElementText();
                continue;
            }

            // quit reading when basic elements are read
            if (!d->m_identifier.isEmpty()
                && !d->m_title.isEmpty()
                && !d->m_i18nTitle.isEmpty()
                && !d->m_description.isEmpty()
                && !d->m_languageId.isEmpty()
            )
            {
                break;
            }
        }
        if (xml.hasError()) {
            qCritical() << "Error occurred when reading Course XML file:" << path.toLocalFile();
        }
    } else {
        qCCritical(ARTIKULATE_CORE()) << "Could not open course file" << path.toLocalFile();
    }
    xml.clear();
    file.close();

    // find correct language
    if (repository != nullptr) {
        for (const auto &language : repository->languages()) {
            if (language == nullptr) {
                continue;
            }
            if (language->id() == d->m_languageId) {
                d->m_language = language;
            }
        }
    }
    if (d->m_language == nullptr) {
        qCCritical(ARTIKULATE_CORE()) << "A course with an unknown language was loaded";
    }
}

CourseResource::CourseResource(const QUrl &path, const QVector<Language *> &languages, IResourceRepository *repository)
    : CourseResource(path, repository)
{
    for (const auto &language : languages) {
        if (language == nullptr) {
            continue;
        }
        if (language->id() == d->m_languageId) {
            d->m_language = language;
        }
    }
}

CourseResource::~CourseResource()
{

}

QString CourseResource::id() const
{
    return d->m_identifier;
}

QString CourseResource::title() const
{
    return d->m_title;
}

QString CourseResource::i18nTitle() const
{
    return d->m_i18nTitle;
}

QString CourseResource::description() const
{
    return d->m_description;
}

Language * CourseResource::language() const
{
    return d->m_language;
}

QList<Unit *> CourseResource::unitList()
{
    if (d->m_loadedCourse == nullptr) {
        d->loadCourse();
    }
    if (d->m_loadedCourse != nullptr) {
        return d->m_loadedCourse->unitList();
    }
    return QList<Unit *>();
}

void CourseResource::sync()
{
    Q_ASSERT(d->m_path.isValid());
    Q_ASSERT(d->m_path.isLocalFile());
    Q_ASSERT(!d->m_path.isEmpty());

    // if resource was never loaded, it cannot be changed
    if (d->m_loadedCourse == nullptr) {
        qCDebug(ARTIKULATE_LOG()) << "Aborting sync, course was not parsed.";
        return;
    }

     // not writing back if not modified
     if (!d->m_loadedCourse->modified()) {
         qCDebug(ARTIKULATE_LOG()) << "Aborting sync, course was not modified.";
         return;
     }

    // write back to file
    // create directories if necessary
    QFileInfo info(d->m_path.adjusted(QUrl::RemoveFilename|QUrl::StripTrailingSlash).path());
    if (!info.exists()) {
        qCDebug(ARTIKULATE_LOG) << "create xml output file directory, not existing";
        QDir dir;
        dir.mkpath(d->m_path.adjusted(QUrl::RemoveFilename|QUrl::StripTrailingSlash).path());
    }

    //TODO port to KSaveFile
    QFile file(d->m_path.toLocalFile());
    if (!file.open(QIODevice::WriteOnly)) {
        qCWarning(ARTIKULATE_LOG) << "Unable to open file " << file.fileName() << " in write mode, aborting.";
        return;
    }

    file.write(serializedDocument().toByteArray());
    return;
}

void CourseResource::exportGhns(const QString &path)
{
    if (d->m_loadedCourse == nullptr) {
        d->loadCourse();
    }

    // filename
    const QString fileName = id() + ".tar.bz2";
    KTar tar = KTar(path + '/' + fileName, QStringLiteral("application/x-bzip"));
    if (!tar.open(QIODevice::WriteOnly)) {
        qCWarning(ARTIKULATE_LOG) << "Unable to open tar file"
            << path + '/' + fileName
            << "in write mode, aborting.";
        return;
    }

    for (auto *unit : unitList()) {
        for (auto *phrase : unit->phraseList()) {
            if (QFile::exists(phrase->soundFileUrl())) {
                tar.addLocalFile(phrase->soundFileUrl(), phrase->id() + ".ogg");
            }
        }
    }

    tar.writeFile(id() + ".xml", serializedDocument(true).toByteArray());

    tar.close();
}

void CourseResource::close()
{
    d->m_loadedCourse->deleteLater();
    d->m_loadedCourse = nullptr;
}

bool CourseResource::isOpen() const
{
    return (d->m_loadedCourse != nullptr);
}

QUrl CourseResource::file() const
{
    if (d->m_loadedCourse) {
        return d->m_loadedCourse->file();
    }
    return d->m_path;
}

void CourseResourcePrivate::loadCourse()
{
    if (m_loadedCourse != nullptr) {
        qCWarning(ARTIKULATE_CORE()) << "Skipping loading of course, no reloading implemented yet";
        return;
    }

    QFileInfo info(m_path.toLocalFile());
    if (!info.exists()) {
        qCCritical(ARTIKULATE_LOG()) << "No course file available at location" << m_path.toLocalFile();
        return;
    }

    // load existing file
    QXmlSchema schema = loadXmlSchema(QStringLiteral("course"));
    if (!schema.isValid()) {
        qCWarning(ARTIKULATE_CORE()) << "Scheme not valid, aborting";
        return;
    }
    QDomDocument document = loadDomDocument(m_path, schema);
    if (document.isNull()) {
        qCWarning(ARTIKULATE_CORE()) << "Could not parse document " << m_path.toLocalFile() << ", aborting.";
        return;
    }

    // create course
    QDomElement root(document.documentElement());
    m_loadedCourse = new Course(nullptr);

    m_loadedCourse->setFile(m_path);
    m_loadedCourse->setId(root.firstChildElement(QStringLiteral("id")).text());
    m_loadedCourse->setTitle(root.firstChildElement(QStringLiteral("title")).text());
    m_loadedCourse->setDescription(root.firstChildElement(QStringLiteral("description")).text());
    if (!root.firstChildElement(QStringLiteral("foreignId")).isNull()) {
        m_loadedCourse->setForeignId(root.firstChildElement(QStringLiteral("foreignId")).text());
    }

    // set language
    //TODO not efficient to load completely every language for this comparison
    QString languageId = root.firstChildElement(QStringLiteral("language")).text();
    for(const auto &language : m_repository->languages()) {
        if (language->id() == languageId) {
            m_loadedCourse->setLanguage(language);
            break;
        }
    }
    if (m_loadedCourse->language() == nullptr) {
        qCWarning(ARTIKULATE_LOG) << "Language ID" << languageId << "unknown, could not register any language, aborting";
        return;
    }

    // create units
    for (QDomElement unitNode = root.firstChildElement(QStringLiteral("units")).firstChildElement();
         !unitNode.isNull();
         unitNode = unitNode.nextSiblingElement())
    {
        Unit *unit = new Unit(nullptr);
        unit->setId(unitNode.firstChildElement(QStringLiteral("id")).text());
        unit->setCourse(m_loadedCourse);
        unit->setTitle(unitNode.firstChildElement(QStringLiteral("title")).text());
        if (!unitNode.firstChildElement(QStringLiteral("foreignId")).isNull()) {
            unit->setForeignId(unitNode.firstChildElement(QStringLiteral("foreignId")).text());
        }
        m_loadedCourse->addUnit(unit);

        // create phrases
        for (QDomElement phraseNode = unitNode.firstChildElement(QStringLiteral("phrases")).firstChildElement();
            !phraseNode.isNull();
            phraseNode = phraseNode.nextSiblingElement())
        {
            unit->addPhrase(parsePhrase(phraseNode, unit)); // add to unit at last step to produce only one signal
            //FIXME phrase does not cause unit signals that phonemes list is changed
        }
    }
    m_loadedCourse->setModified(false);
}

Course * CourseResource::course()
{
    return nullptr;
}

Phrase* CourseResourcePrivate::parsePhrase(QDomElement phraseNode, Unit* parentUnit) const
{
    Phrase *phrase = new Phrase(parentUnit);
    phrase->setId(phraseNode.firstChildElement(QStringLiteral("id")).text());
    phrase->setText(phraseNode.firstChildElement(QStringLiteral("text")).text());
    phrase->seti18nText(phraseNode.firstChildElement(QStringLiteral("i18nText")).text());
    phrase->setUnit(parentUnit);
    if (!phraseNode.firstChildElement(QStringLiteral("soundFile")).text().isEmpty()) {
        phrase->setSound(QUrl::fromLocalFile(
                m_path.adjusted(QUrl::RemoveFilename|QUrl::StripTrailingSlash).path()
                + '/' + phraseNode.firstChildElement(QStringLiteral("soundFile")).text())
            );
    }
    phrase->setType(phraseNode.firstChildElement(QStringLiteral("type")).text());
    phrase->setEditState(phraseNode.firstChildElement(QStringLiteral("editState")).text());
    if (!phraseNode.firstChildElement(QStringLiteral("foreignId")).isNull()) {
        phrase->setForeignId(phraseNode.firstChildElement(QStringLiteral("foreignId")).text());
    }

    // add phonemes
    QList<Phoneme *> phonemes = m_loadedCourse->language()->phonemes();
    for (QDomElement phonemeID = phraseNode.firstChildElement(QStringLiteral("phonemes")).firstChildElement();
        !phonemeID.isNull();
            phonemeID = phonemeID.nextSiblingElement())
    {
        QString id = phonemeID.text();
        if (id.isEmpty()) {
            qCritical() << "Phoneme ID string is empty for phrase "<< phrase->id() <<", aborting.";
            continue;
        }
        foreach (Phoneme *phoneme, phonemes) {
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

QDomDocument CourseResource::serializedDocument(bool trainingExport) const
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

    idElement.appendChild(document.createTextNode(d->m_loadedCourse->id()));
    titleElement.appendChild(document.createTextNode(d->m_loadedCourse->title()));
    descriptionElement.appendChild(document.createTextNode(d->m_loadedCourse->description()));
    languageElement.appendChild(document.createTextNode(d->m_loadedCourse->language()->id()));

    QDomElement unitListElement = document.createElement(QStringLiteral("units"));
    // create units
    foreach (Unit *unit, d->m_loadedCourse->unitList()) {
        QDomElement unitElement = document.createElement(QStringLiteral("unit"));

        QDomElement unitIdElement = document.createElement(QStringLiteral("id"));
        QDomElement unitTitleElement = document.createElement(QStringLiteral("title"));
        QDomElement unitPhraseListElement = document.createElement(QStringLiteral("phrases"));
        unitIdElement.appendChild(document.createTextNode(unit->id()));
        unitTitleElement.appendChild(document.createTextNode(unit->title()));

        // construct phrases
        foreach (Phrase *phrase, unit->phraseList()) {
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
    if (!d->m_loadedCourse->foreignId().isEmpty()) {
        QDomElement courseForeignIdElement = document.createElement(QStringLiteral("foreignId"));
        courseForeignIdElement.appendChild(document.createTextNode(d->m_loadedCourse->foreignId()));
        root.appendChild(courseForeignIdElement);
    }
    root.appendChild(titleElement);
    root.appendChild(descriptionElement);
    root.appendChild(languageElement);
    root.appendChild(unitListElement);

    return document;
}

QDomElement CourseResource::serializedPhrase(Phrase *phrase, QDomDocument &document) const
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
