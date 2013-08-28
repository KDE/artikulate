/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include <KDebug>

class CourseResourcePrivate
{
public:
    CourseResourcePrivate(ResourceManager *resourceManager)
        : m_resourceManager(resourceManager)
        , m_courseResource(0)
    {
    }

    ~CourseResourcePrivate()
    {
    }

    ResourceManager *m_resourceManager;
    KUrl m_path;
    ResourceInterface::Type m_type;
    QString m_identifier;
    QString m_title;
    QString m_language;
    QString m_i18nTitle;
    Course *m_courseResource;
};

CourseResource::CourseResource(ResourceManager *resourceManager, const KUrl &path)
    : ResourceInterface(resourceManager)
    , d(new CourseResourcePrivate(resourceManager))
{
    d->m_type = ResourceInterface::CourseResourceType;
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
            kError() << "Error occured when reading Course XML file:" << path.toLocalFile();
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
    if (d->m_courseResource == 0) {
        kDebug() << "Aborting sync, course was not parsed.";
        return;
    }

//     // not writing back if not modified
//     if (!d->m_courseResource->modified()) {
//         kDebug() << "Aborting sync, course was not modified.";
//         return;
//     }

    QDomDocument document;
    // prepare xml header
    QDomProcessingInstruction header = document.createProcessingInstruction("xml", "version=\"1.0\"");
    document.appendChild(header);

    // create main element
    QDomElement root = document.createElement("course");
    document.appendChild(root);

    QDomElement idElement = document.createElement("id");
    QDomElement titleElement = document.createElement("title");
    QDomElement descriptionElement = document.createElement("description");
    QDomElement languageElement = document.createElement("language");

    idElement.appendChild(document.createTextNode(d->m_courseResource->id()));
    titleElement.appendChild(document.createTextNode(d->m_courseResource->title()));
    descriptionElement.appendChild(document.createTextNode(d->m_courseResource->description()));
    languageElement.appendChild(document.createTextNode(d->m_courseResource->language()->id()));

    QDomElement unitListElement = document.createElement("units");
    // create units
    foreach (Unit *unit, d->m_courseResource->unitList()) {
        QDomElement unitElement = document.createElement("unit");

        QDomElement unitIdElement = document.createElement("id");
        QDomElement unitTitleElement = document.createElement("title");
        QDomElement unitPhraseListElement = document.createElement("phrases");
        unitIdElement.appendChild(document.createTextNode(unit->id()));
        unitTitleElement.appendChild(document.createTextNode(unit->title()));

        // construct phrases
        foreach (Phrase *phrase, unit->phraseList()) {
            QDomElement phraseElement = document.createElement("phrase");
            QDomElement phraseIdElement = document.createElement("id");
            QDomElement phraseTextElement = document.createElement("text");
            QDomElement phrasei18nTextElement = document.createElement("i18nText");
            QDomElement phraseSoundFileElement = document.createElement("soundFile");
            QDomElement phraseTypeElement = document.createElement("type");
            QDomElement phraseEditStateElement = document.createElement("editState");
            QDomElement phrasePhonemeListElement = document.createElement("phonemes");

            phraseIdElement.appendChild(document.createTextNode(phrase->id()));
            phraseTextElement.appendChild(document.createTextNode(phrase->text()));
            phrasei18nTextElement.appendChild(document.createTextNode(phrase->i18nText()));
            phraseSoundFileElement.appendChild(document.createTextNode(phrase->sound().fileName()));
            phraseTypeElement.appendChild(document.createTextNode(phrase->typeString()));
            phraseEditStateElement.appendChild(document.createTextNode(phrase->editStateString()));

            // add phonemes
            foreach (Phoneme *phoneme, phrase->phonemes()) {
                QDomElement phonemeElement = document.createElement("phonemeID");
                phonemeElement.appendChild(document.createTextNode(phoneme->id()));
                phrasePhonemeListElement.appendChild(phonemeElement);
            }

            phraseElement.appendChild(phraseIdElement);
            if (!phrase->foreignId().isEmpty()) {
                QDomElement phraseForeignIdElement = document.createElement("foreignId");
                phraseForeignIdElement.appendChild(document.createTextNode(phrase->foreignId()));
                phraseElement.appendChild(phraseForeignIdElement);
            }
            phraseElement.appendChild(phraseTextElement);
            phraseElement.appendChild(phrasei18nTextElement);
            phraseElement.appendChild(phraseSoundFileElement);
            phraseElement.appendChild(phraseTypeElement);
            phraseElement.appendChild(phraseEditStateElement);
            phraseElement.appendChild(phrasePhonemeListElement);

            unitPhraseListElement.appendChild(phraseElement);
        }

        // construct the unit element
        unitElement.appendChild(unitIdElement);
        if (!unit->foreignId().isEmpty()) {
            QDomElement unitForeignIdElement = document.createElement("foreignId");
            unitForeignIdElement.appendChild(document.createTextNode(unit->foreignId()));
            unitElement.appendChild(unitForeignIdElement);
        }
        unitElement.appendChild(unitTitleElement);
        unitElement.appendChild(unitPhraseListElement);

        unitListElement.appendChild(unitElement);
    }

    root.appendChild(idElement);
    if (!d->m_courseResource->foreignId().isEmpty()) {
        QDomElement courseForeignIdElement = document.createElement("foreignId");
        courseForeignIdElement.appendChild(document.createTextNode(d->m_courseResource->foreignId()));
        root.appendChild(courseForeignIdElement);
    }
    root.appendChild(titleElement);
    root.appendChild(descriptionElement);
    root.appendChild(languageElement);
    root.appendChild(unitListElement);

    // write back to file
    //TODO port to KSaveFile
    QFile file;
    file.setFileName(path().toLocalFile());
    if (!file.open(QIODevice::WriteOnly)) {
        kWarning() << "Unable to open file " << file.fileName() << " in write mode, aborting.";
        return;
    }

    file.write(document.toByteArray());
    return;
}

void CourseResource::close()
{
    d->m_courseResource->deleteLater();
    d->m_courseResource = 0;
}

bool CourseResource::isOpen() const
{
    return (d->m_courseResource != 0);
}

KUrl CourseResource::path() const
{
    if (d->m_courseResource) {
        return d->m_courseResource->file();
    }
    return d->m_path;
}

QObject * CourseResource::resource()
{
    if (d->m_courseResource != 0) {
        return d->m_courseResource;
    }

    if (!path().isLocalFile()) {
        kWarning() << "Cannot open course file at " << path().toLocalFile() << ", aborting.";
        return 0;
    }

    QXmlSchema schema = loadXmlSchema("course");
    if (!schema.isValid()) {
        return 0;
    }

    QDomDocument document = loadDomDocument(path(), schema);
    if (document.isNull()) {
        kWarning() << "Could not parse document " << path().toLocalFile() << ", aborting.";
        return 0;
    }

    // create course
    QDomElement root(document.documentElement());
    d->m_courseResource = new Course(this);

    d->m_courseResource->setFile(d->m_path);
    d->m_courseResource->setId(root.firstChildElement("id").text());
    d->m_courseResource->setTitle(root.firstChildElement("title").text());
    d->m_courseResource->setDescription(root.firstChildElement("description").text());
    if (!root.firstChildElement("foreignId").isNull()) {
        d->m_courseResource->setForeignId(root.firstChildElement("foreignId").text());
    }

    // set language
    //TODO not efficient to load completely every language for this comparison
    QString language = root.firstChildElement("language").text();
    foreach(LanguageResource * resource, d->m_resourceManager->languageResources()) {
        if (resource->language()->id() == language) {
            d->m_courseResource->setLanguage(resource->language());
            break;
        }
    }
    if (d->m_courseResource->language() == 0) {
        kWarning() << "Language ID" << language << "unknown, could not register any language, aborting";
        return 0;
    }

    // create units
    for (QDomElement unitNode = root.firstChildElement("units").firstChildElement();
         !unitNode.isNull();
         unitNode = unitNode.nextSiblingElement())
    {
        Unit *unit = new Unit(d->m_courseResource);
        unit->setId(unitNode.firstChildElement("id").text());
        unit->setCourse(d->m_courseResource);
        unit->setTitle(unitNode.firstChildElement("title").text());
        if (!unitNode.firstChildElement("foreignId").isNull()) {
            unit->setForeignId(unitNode.firstChildElement("foreignId").text());
        }
        d->m_courseResource->addUnit(unit);

        // create phrases
        for (QDomElement phraseNode = unitNode.firstChildElement("phrases").firstChildElement();
            !phraseNode.isNull();
            phraseNode = phraseNode.nextSiblingElement())
        {
            Phrase *phrase = new Phrase(unit);
            phrase->setId(phraseNode.firstChildElement("id").text());
            phrase->setText(phraseNode.firstChildElement("text").text());
            phrase->seti18nText(phraseNode.firstChildElement("i18nText").text());
            phrase->setUnit(unit);
            if (!phraseNode.firstChildElement("soundFile").text().isEmpty()) {
                phrase->setSound(KUrl::fromLocalFile(
                        path().directory() + '/' + phraseNode.firstChildElement("soundFile").text())
                    );
            }
            phrase->setType(phraseNode.firstChildElement("type").text());
            phrase->setEditState(phraseNode.firstChildElement("editState").text());
            if (!phraseNode.firstChildElement("foreignId").isNull()) {
                phrase->setForeignId(phraseNode.firstChildElement("foreignId").text());
            }

            // add phonemes
            QList<Phoneme *> phonemes = d->m_courseResource->language()->phonemes();
            for (QDomElement phonemeID = phraseNode.firstChildElement("phonemes").firstChildElement();
                !phonemeID.isNull();
                    phonemeID = phonemeID.nextSiblingElement())
            {
                QString id = phonemeID.text();
                if (id.isEmpty()) {
                    kError() << "Phoneme ID string is empty for phrase "<< phrase->id() <<", aborting.";
                    continue;
                }
                foreach (Phoneme *phoneme, phonemes) {
                    if (phoneme->id() == id) {
                        phrase->addPhoneme(phoneme);
                        break;
                    }
                }
            }
            unit->addPhrase(phrase); // add to unit at last step to produce only one signal
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
