/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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

#include "resourcemanager.h"
#include "language.h"
#include "tag.h"
#include "taggroup.h"

#include <QIODevice>
#include <QFile>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QDomDocument>

#include <KGlobal>
#include <KStandardDirs>
#include <KDebug>
#include <KUrl>

ResourceManager::ResourceManager(QObject *parent)
    : QObject(parent)
{
}

Language * ResourceManager::loadLanguage(const KUrl &languageFile)
{
    if (languageFile.isLocalFile()) {
        kWarning() << "Cannot open language file at " << languageFile.toLocalFile() << ", aborting.";
        return 0;
    }

    QXmlSchema schema = loadXmlSchema("language");
    if (!schema.isValid()) {
        return 0;
    }

    QDomDocument document = loadDomDocument(languageFile, schema);
    if (document.isNull()) {
        kWarning() << "Could not parse document " << languageFile.toLocalFile() << ", aborting.";
        return 0;
    }

    QDomElement root(document.documentElement());
    Language *language = new Language();
    language->setFile(languageFile);
    language->setId(root.firstChildElement("id").text());
    language->setTitle(root.firstChildElement("title").text());

    // create tags
    for (QDomElement tagNode = root.firstChildElement("tags").firstChildElement();
         !tagNode.isNull();
         tagNode = tagNode.nextSiblingElement())
    {
        language->addPrononciationTag(tagNode.attribute("id"), tagNode.attribute("title"));
    }
    QList<Tag *> tags = language->prononciationTags();

    // create tag groups
    for (QDomElement groupNode = root.firstChildElement("tags").firstChildElement();
         !groupNode.isNull();
         groupNode = groupNode.nextSiblingElement())
    {
        TagGroup *group = language->addPrononciationGroup(groupNode.attribute("id"), groupNode.attribute("title"));
        group->setDescription(groupNode.attribute("description"));
        // register tags
        for (QDomElement tagNode = groupNode.firstChildElement("tags").firstChildElement();
            !tagNode.isNull();
            tagNode = tagNode.nextSiblingElement())
        {
            QString id = tagNode.attribute("id");
            foreach (Tag *tag, tags) {
                if (tag->id() == id) {
                    group->addTag(tag);
                    break;
                }
            }
        }
    }

    return language;
}

QXmlSchema ResourceManager::loadXmlSchema(const QString &schemeName)
{
    QString relPath = QString("schemes/%1.xsd").arg(schemeName);
    KUrl file = KUrl::fromLocalFile(KGlobal::dirs()->findResource("appdata", relPath));

    QXmlSchema schema;
    if (schema.load(file) == true) {
        kWarning() << "Schema at file " << file.toLocalFile() << " is invalid.";
    }
    return schema;
}

QDomDocument ResourceManager::loadDomDocument(const KUrl &file, const QXmlSchema &schema)
{
    QDomDocument document;
    QXmlSchemaValidator validator(schema);
    if (!validator.validate(file)) {
        kWarning() << "Schema is not valid, aborting loading of XML document.";
        return document;
    }

    QString errorMsg;
    if (!document.setContent(file.toLocalFile(), &errorMsg)) {
        kWarning() << errorMsg;
    }
    return document;
}
