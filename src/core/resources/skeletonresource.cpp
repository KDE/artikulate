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

#include "skeletonresource.h"
#include "courseparser.h"
#include "core/language.h"
#include "core/skeleton.h"
#include "core/unit.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"
#include "core/resources/languageresource.h"

#include <QXmlSchema>
#include <QXmlStreamReader>
#include <QDomDocument>
#include <QIODevice>
#include <QFile>

#include "artikulate_debug.h"

class SkeletonResourcePrivate
{
public:
    SkeletonResourcePrivate()
        : m_skeletonResource(nullptr)
    {
    }

    ~SkeletonResourcePrivate() = default;

    QUrl m_path;
    QString m_identifier;
    QString m_title;
    QString m_i18nTitle;
    Skeleton *m_skeletonResource;
};

SkeletonResource::SkeletonResource(const QUrl &path)
    : QObject()
    , d(new SkeletonResourcePrivate)
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

            // quit reading when basic elements are read
            if (!d->m_identifier.isEmpty()
                && !d->m_title.isEmpty()
                && !d->m_i18nTitle.isEmpty()
            )
            {
                break;
            }
        }
        if (xml.hasError()) {
            qCritical() << "Error occurred when reading Skeleton XML file:" << path.toLocalFile();
        }
    }
    xml.clear();
    file.close();
}

SkeletonResource::SkeletonResource(Skeleton *skeleton)
    : QObject()
    , d(new SkeletonResourcePrivate)
{
    d->m_path = skeleton->file();
    d->m_identifier = skeleton->id();
    d->m_title = skeleton->title();
    d->m_skeletonResource = skeleton;
}

SkeletonResource::~SkeletonResource()
{

}

QString SkeletonResource::identifier()
{
    if (d->m_skeletonResource) {
        return d->m_skeletonResource->id();
    }
    return d->m_identifier;
}

QString SkeletonResource::title()
{
    if (d->m_skeletonResource) {
        return d->m_skeletonResource->title();
    }
    return d->m_title;
}

QString SkeletonResource::i18nTitle()
{
    if (d->m_skeletonResource) {
        return d->m_skeletonResource->title(); //TODO
    }
    return d->m_i18nTitle;
}

void SkeletonResource::close()
{
    d->m_skeletonResource->deleteLater();
    d->m_skeletonResource = nullptr;
}

void SkeletonResource::sync()
{
    Q_ASSERT(path().isValid());
    Q_ASSERT(path().isLocalFile());
    Q_ASSERT(!path().isEmpty());

    // if resource was never loaded, it cannot be changed
    if (!d->m_skeletonResource) {
        qCDebug(ARTIKULATE_LOG) << "Aborting sync, skeleton was not parsed.";
        return;
    }

//     // not writing back if not modified
//     if (!d->m_skeletonResource->modified()) {
//         qCDebug(ARTIKULATE_LOG) << "Aborting sync, skeleton was not modified.";
//         return;
//     }

    QDomDocument document;
    // prepare xml header
    QDomProcessingInstruction header = document.createProcessingInstruction(QStringLiteral("xml"), QStringLiteral("version=\"1.0\""));
    document.appendChild(header);

    // create main element
    QDomElement root = document.createElement(QStringLiteral("skeleton"));
    document.appendChild(root);

    QDomElement idElement = document.createElement(QStringLiteral("id"));
    QDomElement titleElement = document.createElement(QStringLiteral("title"));
    QDomElement descriptionElement = document.createElement(QStringLiteral("description"));

    idElement.appendChild(document.createTextNode(d->m_skeletonResource->id()));
    titleElement.appendChild(document.createTextNode(d->m_skeletonResource->title()));
    descriptionElement.appendChild(document.createTextNode(d->m_skeletonResource->description()));

    QDomElement unitListElement = document.createElement(QStringLiteral("units"));
    // create units
    foreach (Unit *unit, d->m_skeletonResource->unitList()) {
        QDomElement unitElement = document.createElement(QStringLiteral("unit"));

        QDomElement unitIdElement = document.createElement(QStringLiteral("id"));
        QDomElement unitTitleElement = document.createElement(QStringLiteral("title"));
        QDomElement unitPhraseListElement = document.createElement(QStringLiteral("phrases"));
        unitIdElement.appendChild(document.createTextNode(unit->id()));
        unitTitleElement.appendChild(document.createTextNode(unit->title()));

        // construct phrases
        foreach (Phrase *phrase, unit->phraseList()) {
            QDomElement phraseElement = document.createElement(QStringLiteral("phrase"));
            QDomElement phraseIdElement = document.createElement(QStringLiteral("id"));
            QDomElement phraseTextElement = document.createElement(QStringLiteral("text"));
            QDomElement phraseTypeElement = document.createElement(QStringLiteral("type"));

            phraseIdElement.appendChild(document.createTextNode(phrase->id()));
            phraseTextElement.appendChild(document.createTextNode(phrase->text()));
            phraseTypeElement.appendChild(document.createTextNode(phrase->typeString()));

            phraseElement.appendChild(phraseIdElement);
            phraseElement.appendChild(phraseTextElement);
            phraseElement.appendChild(phraseTypeElement);

            unitPhraseListElement.appendChild(phraseElement);
        }

        // construct the unit element
        unitElement.appendChild(unitIdElement);
        unitElement.appendChild(unitTitleElement);
        unitElement.appendChild(unitPhraseListElement);

        unitListElement.appendChild(unitElement);
    }

    root.appendChild(idElement);
    root.appendChild(titleElement);
    root.appendChild(descriptionElement);
    root.appendChild(unitListElement);


    // write back to file
    //TODO port to KSaveFile
    QFile file(path().toLocalFile());
    if (!file.open(QIODevice::WriteOnly)) {
        qCWarning(ARTIKULATE_LOG) << "Unable to open file " << file.fileName() << " in write mode, aborting.";
        return;
    }

    file.write(document.toByteArray());
    return;
}

void SkeletonResource::reload()
{
    qCritical() << "NOT IMPLEMENTED";
}

bool SkeletonResource::isOpen() const
{
    return (d->m_skeletonResource != nullptr);
}

QUrl SkeletonResource::path() const
{
    if (d->m_skeletonResource) {
        return d->m_skeletonResource->file();
    }
    return d->m_path;
}

QObject * SkeletonResource::resource()
{
    if (d->m_skeletonResource) {
        return d->m_skeletonResource;
    }

    if (!path().isLocalFile()) {
        qCWarning(ARTIKULATE_LOG) << "Cannot open skeleton file at " << path().toLocalFile() << ", aborting.";
        return nullptr;
    }

    QXmlSchema schema = CourseParser::loadXmlSchema(QStringLiteral("skeleton"));
    if (!schema.isValid()) {
        return nullptr;
    }

    QDomDocument document = CourseParser::loadDomDocument(path(), schema);
    if (document.isNull()) {
        qCWarning(ARTIKULATE_LOG) << "Could not parse document " << path().toLocalFile() << ", aborting.";
        return nullptr;
    }

    // create skeleton
    QDomElement root(document.documentElement());
    d->m_skeletonResource = new Skeleton();

    d->m_skeletonResource->setFile(d->m_path);
    d->m_skeletonResource->setId(root.firstChildElement(QStringLiteral("id")).text());
    d->m_skeletonResource->setTitle(root.firstChildElement(QStringLiteral("title")).text());
    d->m_skeletonResource->setDescription(root.firstChildElement(QStringLiteral("title")).text());

    // create units
    for (QDomElement unitNode = root.firstChildElement(QStringLiteral("units")).firstChildElement();
         !unitNode.isNull();
         unitNode = unitNode.nextSiblingElement())
    {
        Unit *unit = new Unit(d->m_skeletonResource);
        unit->setId(unitNode.firstChildElement(QStringLiteral("id")).text());
        unit->setCourse(d->m_skeletonResource);
        unit->setTitle(unitNode.firstChildElement(QStringLiteral("title")).text());
        d->m_skeletonResource->addUnit(unit);

        // create phrases
        for (QDomElement phraseNode = unitNode.firstChildElement(QStringLiteral("phrases")).firstChildElement();
            !phraseNode.isNull();
            phraseNode = phraseNode.nextSiblingElement())
        {
            Phrase *phrase = new Phrase(unit);
            phrase->setId(phraseNode.firstChildElement(QStringLiteral("id")).text());
            phrase->setText(phraseNode.firstChildElement(QStringLiteral("text")).text());
            phrase->setType(phraseNode.firstChildElement(QStringLiteral("type")).text());
            phrase->setUnit(unit);

            unit->addPhrase(phrase);
        }
    }
    d->m_skeletonResource->setModified(false);

    return d->m_skeletonResource;
}

Skeleton * SkeletonResource::skeleton()
{
    return qobject_cast<Skeleton*>(resource());
}
