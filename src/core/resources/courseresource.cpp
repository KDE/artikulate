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
#include "core/resources/languageresource.h"

#include <QXmlSchema>
#include <QXmlStreamReader>
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
    CourseResourcePrivate(ResourceManager *resourceManager)
        : m_resourceManager(resourceManager)
        , m_type(ResourceInterface::CourseResourceType)
        , m_courseResource(nullptr)
    {
    }

    ~CourseResourcePrivate()
    {
    }

    ResourceManager *m_resourceManager;
    QUrl m_path;
    ResourceInterface::Type m_type;
    QString m_identifier;
    QString m_title;
    QString m_language;
    QString m_i18nTitle;
    Course *m_courseResource;
};

CourseResource::CourseResource(ResourceManager *resourceManager, const QUrl &path)
    : ResourceInterface(resourceManager)
    , d(new CourseResourcePrivate(resourceManager))
{
    d->m_path = path;

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
            if (xml.name() == "title") {
                d->m_title = xml.readElementText();
                d->m_i18nTitle = d->m_title;
                continue;
            }
            //TODO i18nTitle must be implemented, currently missing and hence not parsed
            if (xml.name() == "language") {
                d->m_language = xml.readElementText();
                continue;
            }

            // quit reading when basic elements are read
            if (!d->m_identifier.isEmpty()
                && !d->m_title.isEmpty()
                && !d->m_i18nTitle.isEmpty()
                && !d->m_language.isEmpty()
            )
            {
                break;
            }
        }
        if (xml.hasError()) {
            qCritical() << "Error occurred when reading Course XML file:" << path.toLocalFile();
        }
    }
    xml.clear();
    file.close();
}

CourseResource::~CourseResource()
{

}

QString CourseResource::identifier()
{
    if (d->m_courseResource) {
        return d->m_courseResource->id();
    }
    return d->m_identifier;
}

QString CourseResource::title()
{
    if (d->m_courseResource) {
        return d->m_courseResource->title();
    }
    return d->m_title;
}

QString CourseResource::i18nTitle()
{
    if (d->m_courseResource) {
        return d->m_courseResource->title(); //TODO
    }
    return d->m_i18nTitle;
}

QString CourseResource::language() const
{
    if (d->m_courseResource) {
        return d->m_courseResource->language()->id();
    }
    return d->m_language;
}

ResourceInterface::Type CourseResource::type() const
{
    return d->m_type;
}

void CourseResource::sync()
{
    Q_ASSERT(path().isValid());
    Q_ASSERT(path().isLocalFile());
    Q_ASSERT(!path().isEmpty());

    // if resource was never loaded, it cannot be changed
    if (d->m_courseResource == nullptr) {
        qCDebug(ARTIKULATE_LOG) << "Aborting sync, course was not parsed.";
        return;
    }

//TODO
//     // not writing back if not modified
//     if (!d->m_courseResource->modified()) {
//         qCDebug(ARTIKULATE_LOG) << "Aborting sync, course was not modified.";
//         return;
//     }

    // write back to file
    // create directories if necessary
    QFileInfo info(path().adjusted(QUrl::RemoveFilename|QUrl::StripTrailingSlash).path());
    if (!info.exists()) {
        qCDebug(ARTIKULATE_LOG) << "create xml output file directory, not existing";
        QDir dir;
        dir.mkpath(path().adjusted(QUrl::RemoveFilename|QUrl::StripTrailingSlash).path());
    }

    //TODO port to KSaveFile
    QFile file(path().toLocalFile());
    if (!file.open(QIODevice::WriteOnly)) {
        qCWarning(ARTIKULATE_LOG) << "Unable to open file " << file.fileName() << " in write mode, aborting.";
        return;
    }

    file.write(serializedDocument().toByteArray());
    return;
}

void CourseResource::exportGhns(const QString &path)
{
    // ensure that course is loaded before exporting it
    Course *course = CourseResource::course();

    // filename
    const QString fileName = identifier() + ".tar.bz2";
    KTar tar = KTar(path + '/' + fileName, QStringLiteral("application/x-bzip"));
    if (!tar.open(QIODevice::WriteOnly)) {
        qCWarning(ARTIKULATE_LOG) << "Unable to open tar file"
            << path + '/' + fileName
            << "in write mode, aborting.";
        return;
    }

    foreach (auto *unit, course->unitList()) {
        foreach (auto *phrase, unit->phraseList()) {
            if (QFile::exists(phrase->soundFileUrl())) {
                tar.addLocalFile(phrase->soundFileUrl(), phrase->id() + ".ogg");
            }
        }
    }

    tar.writeFile(identifier() + ".xml", serializedDocument(true).toByteArray());

    tar.close();
}

void CourseResource::close()
{
    d->m_courseResource->deleteLater();
    d->m_courseResource = nullptr;
}

bool CourseResource::isOpen() const
{
    return (d->m_courseResource != nullptr);
}

QUrl CourseResource::path() const
{
    if (d->m_courseResource) {
        return d->m_courseResource->file();
    }
    return d->m_path;
}

QObject * CourseResource::resource()
{
    if (d->m_courseResource != nullptr) {
        return d->m_courseResource;
    }

    // if file does not exist, create new course
    QFileInfo info(d->m_path.toLocalFile());
    if (!info.exists()) {
        d->m_courseResource = new Course(this);
        d->m_courseResource->setId(d->m_identifier);
        d->m_courseResource->setTitle(d->m_title);
        return d->m_courseResource;
    }

    // load existing file
    QXmlSchema schema = loadXmlSchema(QStringLiteral("course"));
    if (!schema.isValid()) {
        return nullptr;
    }
    QDomDocument document = loadDomDocument(path(), schema);
    if (document.isNull()) {
        qCWarning(ARTIKULATE_LOG) << "Could not parse document " << path().toLocalFile() << ", aborting.";
        return nullptr;
    }

    // create course
    QDomElement root(document.documentElement());
    d->m_courseResource = new Course(this);

    d->m_courseResource->setFile(d->m_path);
    d->m_courseResource->setId(root.firstChildElement(QStringLiteral("id")).text());
    d->m_courseResource->setTitle(root.firstChildElement(QStringLiteral("title")).text());
    d->m_courseResource->setDescription(root.firstChildElement(QStringLiteral("description")).text());
    if (!root.firstChildElement(QStringLiteral("foreignId")).isNull()) {
        d->m_courseResource->setForeignId(root.firstChildElement(QStringLiteral("foreignId")).text());
    }

    // set language
    //TODO not efficient to load completely every language for this comparison
    QString language = root.firstChildElement(QStringLiteral("language")).text();
    foreach(LanguageResource * resource, d->m_resourceManager->languageResources()) {
        if (resource->language()->id() == language) {
            d->m_courseResource->setLanguage(resource->language());
            break;
        }
    }
    if (d->m_courseResource->language() == nullptr) {
        qCWarning(ARTIKULATE_LOG) << "Language ID" << language << "unknown, could not register any language, aborting";
        return nullptr;
    }

    // create units
    for (QDomElement unitNode = root.firstChildElement(QStringLiteral("units")).firstChildElement();
         !unitNode.isNull();
         unitNode = unitNode.nextSiblingElement())
    {
        Unit *unit = new Unit(d->m_courseResource);
        unit->setId(unitNode.firstChildElement(QStringLiteral("id")).text());
        unit->setCourse(d->m_courseResource);
        unit->setTitle(unitNode.firstChildElement(QStringLiteral("title")).text());
        if (!unitNode.firstChildElement(QStringLiteral("foreignId")).isNull()) {
            unit->setForeignId(unitNode.firstChildElement(QStringLiteral("foreignId")).text());
        }
        d->m_courseResource->addUnit(unit);

        // create phrases
        for (QDomElement phraseNode = unitNode.firstChildElement(QStringLiteral("phrases")).firstChildElement();
            !phraseNode.isNull();
            phraseNode = phraseNode.nextSiblingElement())
        {
            unit->addPhrase(parsePhrase(phraseNode, unit)); // add to unit at last step to produce only one signal
            //FIXME phrase does not cause unit signals that phonemes list is changed
        }
    }
    d->m_courseResource->setModified(false);

    return d->m_courseResource;
}

Course * CourseResource::course()
{
    return qobject_cast<Course*>(resource());
}

Phrase* CourseResource::parsePhrase(QDomElement phraseNode, Unit* parentUnit) const
{
    Phrase *phrase = new Phrase(parentUnit);
    phrase->setId(phraseNode.firstChildElement(QStringLiteral("id")).text());
    phrase->setText(phraseNode.firstChildElement(QStringLiteral("text")).text());
    phrase->seti18nText(phraseNode.firstChildElement(QStringLiteral("i18nText")).text());
    phrase->setUnit(parentUnit);
    if (!phraseNode.firstChildElement(QStringLiteral("soundFile")).text().isEmpty()) {
        phrase->setSound(QUrl::fromLocalFile(
                path().adjusted(QUrl::RemoveFilename|QUrl::StripTrailingSlash).path()
                + '/' + phraseNode.firstChildElement(QStringLiteral("soundFile")).text())
            );
    }
    phrase->setType(phraseNode.firstChildElement(QStringLiteral("type")).text());
    phrase->setEditState(phraseNode.firstChildElement(QStringLiteral("editState")).text());
    if (!phraseNode.firstChildElement(QStringLiteral("foreignId")).isNull()) {
        phrase->setForeignId(phraseNode.firstChildElement(QStringLiteral("foreignId")).text());
    }

    // add phonemes
    QList<Phoneme *> phonemes = d->m_courseResource->language()->phonemes();
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

    idElement.appendChild(document.createTextNode(d->m_courseResource->id()));
    titleElement.appendChild(document.createTextNode(d->m_courseResource->title()));
    descriptionElement.appendChild(document.createTextNode(d->m_courseResource->description()));
    languageElement.appendChild(document.createTextNode(d->m_courseResource->language()->id()));

    QDomElement unitListElement = document.createElement(QStringLiteral("units"));
    // create units
    foreach (Unit *unit, d->m_courseResource->unitList()) {
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
    if (!d->m_courseResource->foreignId().isEmpty()) {
        QDomElement courseForeignIdElement = document.createElement(QStringLiteral("foreignId"));
        courseForeignIdElement.appendChild(document.createTextNode(d->m_courseResource->foreignId()));
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
