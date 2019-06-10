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

#include "languageresource.h"
#include "courseparser.h"
#include "core/language.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"

#include <QXmlSchema>
#include <QXmlStreamReader>
#include <QDomDocument>
#include <QIODevice>
#include <QFile>

#include "artikulate_debug.h"

class LanguageResourcePrivate
{
public:
    LanguageResourcePrivate() = default;
    ~LanguageResourcePrivate() = default;

    std::shared_ptr<Language> m_language;
    QString m_identifier;
    QUrl m_path;
    QString m_title;
    QString m_i18nTitle;
};

LanguageResource::LanguageResource(const QUrl &path)
    : d(new LanguageResourcePrivate)
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
            }
            if (xml.name() == "title") {
                d->m_title = xml.readElementText();
            }
            if (xml.name() == "i18nTitle") {
                d->m_i18nTitle = xml.readElementText();
            }
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
            qCritical() << "Error occurred when reading Language XML file:" << path.toLocalFile();
        }
    }
    xml.clear();
    file.close();
}

LanguageResource::~LanguageResource()
{

}

QString LanguageResource::identifier()
{
    return d->m_identifier;
}

QString LanguageResource::title()
{
    return d->m_title;
}

QString LanguageResource::i18nTitle()
{
    return d->m_i18nTitle;
}

void LanguageResource::close()
{
    // do nothing
    // language files are never closed
}

bool LanguageResource::isOpen() const
{
    return (d->m_language != nullptr);
}

QUrl LanguageResource::path() const
{
    return d->m_path;
}

std::shared_ptr<Language> LanguageResource::language()
{
    if (d->m_language) {
        return d->m_language;
    }

    if (!d->m_path.isLocalFile()) {
        qCWarning(ARTIKULATE_LOG) << "Cannot open language file at " << d->m_path.toLocalFile() << ", aborting.";
        return nullptr;
    }

    QXmlSchema schema = CourseParser::loadXmlSchema(QStringLiteral("language"));
    if (!schema.isValid()) {
        return nullptr;
    }

    QDomDocument document = CourseParser::loadDomDocument(d->m_path, schema);
    if (document.isNull()) {
        qCWarning(ARTIKULATE_LOG) << "Could not parse document " << d->m_path.toLocalFile() << ", aborting.";
        return nullptr;
    }

    QDomElement root(document.documentElement());
    d->m_language = std::shared_ptr<Language>(new Language());
    d->m_language->setFile(d->m_path);
    d->m_language->setId(root.firstChildElement(QStringLiteral("id")).text());
    d->m_language->setTitle(root.firstChildElement(QStringLiteral("title")).text());
    d->m_language->seti18nTitle(root.firstChildElement(QStringLiteral("i18nTitle")).text());
    // create phoneme groups
    for (QDomElement groupNode = root.firstChildElement(QStringLiteral("phonemeGroups")).firstChildElement();
         !groupNode.isNull();
         groupNode = groupNode.nextSiblingElement())
    {
        PhonemeGroup *group = d->m_language->addPhonemeGroup(
            groupNode.firstChildElement(QStringLiteral("id")).text(),
            groupNode.firstChildElement(QStringLiteral("title")).text());
        group->setDescription(groupNode.attribute(QStringLiteral("description")));
        // register phonemes
        for (QDomElement phonemeNode = groupNode.firstChildElement(QStringLiteral("phonemes")).firstChildElement();
            !phonemeNode.isNull();
            phonemeNode = phonemeNode.nextSiblingElement())
        {
            group->addPhoneme(phonemeNode.firstChildElement(QStringLiteral("id")).text(), phonemeNode.firstChildElement(QStringLiteral("title")).text());
        }
    }

    return d->m_language;
}
