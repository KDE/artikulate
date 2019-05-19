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
#include "core/unit.h"
#include "editablecourseresource.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"
#include "core/resources/languageresource.h"

#include <QDomDocument>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QFile>

#include "artikulate_debug.h"

class SkeletonResourcePrivate
{
public:
    SkeletonResourcePrivate(const QUrl &path)
        : m_path(path)
    {
        // load basic information from language file, but does not parse everything
        QXmlStreamReader xml;
        QFile file(path.toLocalFile());
        if (file.open(QIODevice::ReadOnly)) {
            xml.setDevice(&file);
            xml.readNextStartElement();
            while (xml.readNext() && !xml.atEnd()) {
                if (xml.name() == "id") {
                    m_identifier = xml.readElementText();
                    continue;
                }
                if (xml.name() == "title") {
                    m_title = xml.readElementText();
                    continue;
                }
                if (xml.name() == "description") {
                    m_description = xml.readElementText();
                    continue;
                }

                // quit reading when basic elements are read
                if (!m_identifier.isEmpty()
                    && !m_title.isEmpty()
                    && !m_description.isEmpty()
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
    }

    QVector<Unit *> units() {
        if (m_unitsParsed) {
            return m_units;
        }
        m_units = CourseParser::parseUnits(m_path);
        m_unitsParsed = true;
        return m_units;
    }

    QUrl m_path;
    QString m_identifier;
    QString m_title;
    QString m_description;
    QVector<Unit *> m_units;
    bool m_unitsParsed{ false };
};

SkeletonResource::SkeletonResource(const QUrl &path, IResourceRepository *repository)
    : ICourse()
    , d(new SkeletonResourcePrivate(path))
{
    Q_UNUSED(repository);
}

SkeletonResource::~SkeletonResource() = default;

QString SkeletonResource::id() const
{
    return d->m_identifier;
}

void SkeletonResource::setId(const QString &id)
{
    if (d->m_identifier == id) {
        return;
    }
    d->m_identifier = id;
    emit idChanged();
}

QString SkeletonResource::foreignId() const
{
    return id();
}

QString SkeletonResource::title() const
{
    return d->m_title;
}

void SkeletonResource::setTitle(const QString &title)
{
    if (d->m_title == title) {
        return;
    }
    d->m_title = title;
    emit titleChanged();
}

QString SkeletonResource::i18nTitle() const
{
    // there are no localized titles available
    return title();
}

QString SkeletonResource::description() const
{
    return d->m_description;
}

void SkeletonResource::setDescription(const QString &description)
{
    if (d->m_description == description) {
        return;
    }
    d->m_description = description;
    emit descriptionChanged();
}

void SkeletonResource::addUnit(Unit *unit)
{
    emit unitAboutToBeAdded(unit, d->m_units.count() - 1);
    d->m_units.append(unit);
    emit unitAdded();
}

void SkeletonResource::sync()
{
    Q_ASSERT(file().isValid());
    Q_ASSERT(file().isLocalFile());
    Q_ASSERT(!file().isEmpty());

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

    idElement.appendChild(document.createTextNode(id()));
    titleElement.appendChild(document.createTextNode(title()));
    descriptionElement.appendChild(document.createTextNode(description()));

    QDomElement unitListElement = document.createElement(QStringLiteral("units"));
    // create units
    for (auto unit : d->m_units) {
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
    QFile filePath(file().toLocalFile());
    if (!filePath.open(QIODevice::WriteOnly)) {
        qCWarning(ARTIKULATE_LOG) << "Unable to open file " << filePath.fileName() << " in write mode, aborting.";
        return;
    }

    filePath.write(document.toByteArray());
    return;
}

Language * SkeletonResource::language() const
{
    // skeleton must not have a dedicated language
    return nullptr;
}

QList<Unit *> SkeletonResource::unitList()
{
    return d->units().toList();
}

QUrl SkeletonResource::file() const
{
    return d->m_path;
}
