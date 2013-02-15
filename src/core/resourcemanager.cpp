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
#include <ui/newcoursedialog.h>

#include <QIODevice>
#include <QFile>
#include <QPointer>
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
        addCourse(KUrl::fromLocalFile(file));
    }
}

QList< Language* > ResourceManager::languageList() const
{
    return m_languageList;
}

Language * ResourceManager::language(int index) const
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
    QList< Course* > list;
    QMap<Language *, QList<Course *> >::ConstIterator iter = m_courseList.begin();
    while (iter != m_courseList.end()) {
        list << *iter;
        ++iter;
    }
    return list;
}

QList< Course* > ResourceManager::courseList(Language *language) const
{
    if (!m_courseList.contains(language)) {
        return QList< Course* >();
    }
    return m_courseList[language];
}

Course * ResourceManager::course(Language *language, int index) const
{
    Q_ASSERT(m_courseList.contains(language));
    Q_ASSERT(index >= 0 && index < m_courseList[language].count());

    return m_courseList[language].at(index);
}

Course * ResourceManager::loadCourse(const KUrl &courseFile)
{
    if (!courseFile.isLocalFile()) {
        kWarning() << "Cannot open course file at " << courseFile.toLocalFile() << ", aborting.";
        return 0;
    }

    QXmlSchema schema = loadXmlSchema("course");
    if (!schema.isValid()) {
        return 0;
    }

    QDomDocument document = loadDomDocument(courseFile, schema);
    if (document.isNull()) {
        kWarning() << "Could not parse document " << courseFile.toLocalFile() << ", aborting.";
        return 0;
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
        return 0;
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
            phrase->setSound(KUrl::fromLocalFile(
                courseFile.directory() + '/' + phraseNode.firstChildElement("soundFile").text())
                );
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
    course->setModified(false);

    return course;
}

void ResourceManager::reloadCourse(Course *course)
{
    if (!course) {
        kError() << "Cannot reload non-existing course";
        return;
    }
    if (!course->file().isValid()) {
        kError() << "Cannot reload temporary file, aborting.";
        return;
    }
    KUrl file = course->file();
    removeCourse(course);
    addCourse(file);
}

bool ResourceManager::addCourse(const KUrl &courseFile)
{
    Course *course = loadCourse(courseFile);
    if (course == 0) {
        kError() << "Could not load course, aborting";
        return false;
    }
    addCourse(course);
    return true;
}

void ResourceManager::addCourse(Course *course)
{
    Q_ASSERT(m_languageList.contains(course->language()));

    if (m_courseList.contains(course->language())) {
        emit courseAboutToBeAdded(course, m_courseList[course->language()].count());
    }
    else {
        emit courseAboutToBeAdded(course, 0);
        m_courseList.insert(course->language(), QList<Course*>());
    }
    m_courseList[course->language()].append(course);
    emit courseAdded();
}

void ResourceManager::removeCourse(Course *course)
{
    int index = m_courseList[course->language()].indexOf(course);
    emit courseAboutToBeRemoved(index, index);
    m_courseList[course->language()].removeAt(index);
    emit courseRemoved();
    course->deleteLater();
}

void ResourceManager::newCourseDialog()
{
    QPointer<NewCourseDialog> dialog = new NewCourseDialog(this);
    if (dialog->exec() == QDialog::Accepted ) {
        addCourse(dialog->course());
    }
}

void ResourceManager::sync(Course *course)
{
    Q_ASSERT(course->file().isValid());
    Q_ASSERT(course->file().isLocalFile());
    Q_ASSERT(!course->file().isEmpty());


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

    idElement.appendChild(document.createTextNode(course->id()));
    titleElement.appendChild(document.createTextNode(course->title()));
    descriptionElement.appendChild(document.createTextNode(course->description()));
    languageElement.appendChild(document.createTextNode(course->language()->id()));

    QDomElement unitListElement = document.createElement("units");
    // create units
    foreach (Unit *unit, course->unitList()) {
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
            QDomElement phraseSoundFileElement = document.createElement("soundFile");
            QDomElement phraseTypeElement = document.createElement("type");
            QDomElement phraseTagListElement = document.createElement("tags");

            phraseIdElement.appendChild(document.createTextNode(phrase->id()));
            phraseTextElement.appendChild(document.createTextNode(phrase->text()));
            phraseSoundFileElement.appendChild(document.createTextNode(phrase->sound().toLocalFile()));
            phraseTypeElement.appendChild(document.createTextNode(phrase->typeString()));
            //FIXME write back tags, once they are really implemented

            phraseElement.appendChild(phraseIdElement);
            phraseElement.appendChild(phraseTextElement);
            phraseElement.appendChild(phraseSoundFileElement);
            phraseElement.appendChild(phraseTypeElement);
            phraseElement.appendChild(phraseTagListElement);

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
    root.appendChild(languageElement);
    root.appendChild(unitListElement);


    // write back to file
    //TODO port to KSaveFile
    QFile file;
    file.setFileName(course->file().toLocalFile());
    if (!file.open(QIODevice::WriteOnly)) {
        kWarning() << "Unable to open file " << file.fileName() << " in write mode, aborting.";
        return;
    }

    file.write(document.toByteArray());
    return;
}

QXmlSchema ResourceManager::loadXmlSchema(const QString &schemeName) const
{
    QString relPath = QString("schemes/%1.xsd").arg(schemeName);
    KUrl file = KUrl::fromLocalFile(KGlobal::dirs()->findResource("appdata", relPath));

    QXmlSchema schema;
    if (schema.load(file) == false) {
        kWarning() << "Schema at file " << file.toLocalFile() << " is invalid.";
    }
    return schema;
}

QDomDocument ResourceManager::loadDomDocument(const KUrl &path, const QXmlSchema &schema) const
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
