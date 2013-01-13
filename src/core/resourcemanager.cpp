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
#include "course.h"
#include "unit.h"
#include "phrase.h"
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

void ResourceManager::loadLocalData()
{
    // load local language files
    QStringList languageFiles = KGlobal::dirs()->findAllResources("appdata",QString("languages/*.xml"));
    foreach (const QString &file, languageFiles) {
        loadLanguage(KUrl::fromLocalFile(file));
    }

    // load local course files
    QStringList courseFiles = KGlobal::dirs()->findAllResources("appdata",QString("courses/*.xml"));
    foreach (const QString &file, courseFiles) {
        loadCourse(KUrl::fromLocalFile(file));
    }
}

QList< Language* > ResourceManager::languageList() const
{
    return m_languageList;
}

Language* ResourceManager::language(int index) const
{
    Q_ASSERT (index >= 0 && index < m_languageList.count());
    return m_languageList.at(index);
}

bool ResourceManager::loadLanguage(const KUrl &languageFile)
{
    if (!languageFile.isLocalFile()) {
        kWarning() << "Cannot open language file at " << languageFile.toLocalFile() << ", aborting.";
        return false;
    }

    QXmlSchema schema = loadXmlSchema("language");
    if (!schema.isValid()) {
        return false;
    }

    QDomDocument document = loadDomDocument(languageFile, schema);
    if (document.isNull()) {
        kWarning() << "Could not parse document " << languageFile.toLocalFile() << ", aborting.";
        return false;
    }

    QDomElement root(document.documentElement());
    Language *language = new Language(this);
    m_languageList.append(language);
    emit languageAboutToBeAdded(language, m_languageList.count()-1);
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

    emit languageAdded();
    return true;
}

QList< Course* > ResourceManager::courseList() const
{
    return m_courseList;
}

bool ResourceManager::loadCourse(const KUrl &courseFile)
{
    if (!courseFile.isLocalFile()) {
        kWarning() << "Cannot open course file at " << courseFile.toLocalFile() << ", aborting.";
        return false;
    }

    QXmlSchema schema = loadXmlSchema("course");
    if (!schema.isValid()) {
        return false;
    }

    QDomDocument document = loadDomDocument(courseFile, schema);
    if (document.isNull()) {
        kWarning() << "Could not parse document " << courseFile.toLocalFile() << ", aborting.";
        return false;
    }

    // create course
    QDomElement root(document.documentElement());
    Course *course = new Course(this);
    course->setFile(courseFile);
    course->setId(root.firstChildElement("id").text());
    course->setTitle(root.firstChildElement("title").text());
    course->setDescription(root.firstChildElement("title").text());

    // set language
    QString language = root.firstChildElement("language").text();
    QList<Language*>::ConstIterator iter = m_languageList.constBegin();
    while (iter != m_languageList.constEnd()) {
        if ((*iter)->id() == language) {
            course->setLanguage(*iter);
            break;
        }
        ++iter;
    }
    if (course->language() == 0) {
        kWarning() << "Language ID unknown, could not register any language, aborting";
        return false;
    }

    // create units
    for (QDomElement unitNode = root.firstChildElement("units").firstChildElement();
         !unitNode.isNull();
         unitNode = unitNode.nextSiblingElement())
    {
        Unit *unit = new Unit(course);
        course->addUnit(unit);
        unit->setId(unitNode.firstChildElement("id").text());
        unit->setTitle(unitNode.firstChildElement("title").text());

        // create phrases
        for (QDomElement phraseNode = unitNode.firstChildElement("phrases").firstChildElement();
            !phraseNode.isNull();
            phraseNode = phraseNode.nextSiblingElement())
        {
            Phrase *phrase = new Phrase(unit);
            unit->addPhrase(phrase);
            phrase->setId(phraseNode.firstChildElement("id").text());
            phrase->setText(phraseNode.firstChildElement("text").text());
            phrase->setSound(KUrl::fromLocalFile(phraseNode.firstChildElement("soundFile").text()));
            phrase->setType(phraseNode.firstChildElement("type").text());

            // add tags
            QList<Tag *> tags = course->language()->prononciationTags();
            for (QDomElement tagNode = phraseNode.firstChildElement("tags").firstChildElement();
                !tagNode.isNull();
                tagNode = tagNode.nextSiblingElement())
            {
                QString id = tagNode.attribute("id");
                foreach (Tag *tag, tags) {
                    if (tag->id() == id) {
                        phrase->addTag(tag);
                        break;
                    }
                }
            }
        }
    }

    m_courseList.append(course);
    return true;
}

QXmlSchema ResourceManager::loadXmlSchema(const QString &schemeName)
{
    QString relPath = QString("schemes/%1.xsd").arg(schemeName);
    KUrl file = KUrl::fromLocalFile(KGlobal::dirs()->findResource("appdata", relPath));

    QXmlSchema schema;
    if (schema.load(file) == false) {
        kWarning() << "Schema at file " << file.toLocalFile() << " is invalid.";
    }
    return schema;
}

QDomDocument ResourceManager::loadDomDocument(const KUrl &path, const QXmlSchema &schema)
{
    QDomDocument document;
    QXmlSchemaValidator validator(schema);
    if (!validator.validate(path)) {
        kWarning() << "Schema is not valid, aborting loading of XML document.";
        return document;
    }

    QString errorMsg;
    QFile file(path.toLocalFile());
    if (file.open(QIODevice::ReadOnly)) {
        if (!document.setContent(&file, &errorMsg)) {
            kWarning() << errorMsg;
        }
    } else {
        kWarning() << "Could not open XML document " << path.toLocalFile() << " for reading, aborting.";
    }
    return document;
}
