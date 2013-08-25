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
#include "core/resourcemanager.h"
#include "core/language.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"

#include <QXmlSchema>
#include <QDomDocument>

#include <KDebug>

class LanguageResourcePrivate
{
public:
    LanguageResourcePrivate(ResourceManager *resourceManager)
        : m_resourceManager(resourceManager)
        , m_languageResource(0)
    {
    }

    ~LanguageResourcePrivate()
    {
    }

    ResourceManager *m_resourceManager;
    KUrl m_path;
    ResourceInterface::Type m_type;
    QString m_identifier;
    QString m_title;
    Language *m_languageResource;
};

LanguageResource::LanguageResource(ResourceManager *resourceManager, const KUrl &path)
    : ResourceInterface(resourceManager)
    , d(new LanguageResourcePrivate(resourceManager))
{
    d->m_type = ResourceInterface::LanguageResource;
    d->m_path = path;

    //FIXME load basics
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


ResourceInterface::Type LanguageResource::type() const
{
    return d->m_type;
}

void LanguageResource::close()
{
    // do nothing
    // language files are never closed
}

bool LanguageResource::isOpen() const
{
    return (d->m_languageResource != 0);
}

KUrl LanguageResource::path() const
{
    return d->m_path;
}

QObject * LanguageResource::resource()
{
    if (d->m_languageResource != 0) {
        return d->m_languageResource;
    }

    if (!d->m_path.isLocalFile()) {
        kWarning() << "Cannot open language file at " << d->m_path.toLocalFile() << ", aborting.";
        return 0;
    }

    QXmlSchema schema = loadXmlSchema("language");
    if (!schema.isValid()) {
        return 0;
    }

    QDomDocument document = loadDomDocument(d->m_path, schema);
    if (document.isNull()) {
        kWarning() << "Could not parse document " << d->m_path.toLocalFile() << ", aborting.";
        return 0;
    }

    QDomElement root(document.documentElement());
    d->m_languageResource = new Language(this);
    d->m_languageResource->setFile(d->m_path);
    d->m_languageResource->setId(root.firstChildElement("id").text());
    d->m_languageResource->setTitle(root.firstChildElement("title").text());
    d->m_languageResource->seti18nTitle(root.firstChildElement("i18nTitle").text());
    // create phoneme groups
    for (QDomElement groupNode = root.firstChildElement("phonemeGroups").firstChildElement();
         !groupNode.isNull();
         groupNode = groupNode.nextSiblingElement())
    {
        PhonemeGroup *group = d->m_languageResource->addPhonemeGroup(
            groupNode.firstChildElement("id").text(),
            groupNode.firstChildElement("title").text());
        group->setDescription(groupNode.attribute("description"));
        // register phonemes
        for (QDomElement phonemeNode = groupNode.firstChildElement("phonemes").firstChildElement();
            !phonemeNode.isNull();
            phonemeNode = phonemeNode.nextSiblingElement())
        {
            group->addPhoneme(phonemeNode.firstChildElement("id").text(), phonemeNode.firstChildElement("title").text());
        }
    }

    return d->m_languageResource;
}

Language * LanguageResource::language()
{
    return qobject_cast<Language*>(resource());
}
