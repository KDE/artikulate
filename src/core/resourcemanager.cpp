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
#include "skeleton.h"
#include "unit.h"
#include "phrase.h"
#include "phoneme.h"
#include "phonemegroup.h"
#include <ui/newcoursedialog.h>
#include "settings.h"

#include <QIODevice>
#include <QFile>
#include <QPointer>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QDomDocument>
#include <QUuid>
#include <QDir>

#include <KGlobal>
#include <KStandardDirs>
#include <KDebug>
#include <KUrl>

ResourceManager::ResourceManager(QObject *parent)
    : QObject(parent)
{
    updateResourceFileCache();
}

void ResourceManager::updateResourceFileCache()
{
    // find all files and cache paths to them
    QStringList languageFiles = KGlobal::dirs()->findAllResources("appdata",QString("languages/*.xml"));
    foreach (const QString &file, languageFiles) {
        m_languageFileCache.append(KUrl::fromLocalFile(file));
    }
    if (!Settings::useCourseRepository()) {
        QStringList courseFiles = KGlobal::dirs()->findAllResources("appdata",QString("courses/*.xml"));
        foreach (const QString &file, courseFiles) {
            m_courseFileCache.append(KUrl::fromLocalFile(file));
        }
        QStringList skeletonFiles = KGlobal::dirs()->findAllResources("appdata",QString("skeletons/*.xml"));
        foreach (const QString &file, skeletonFiles) {
            m_skeletonFileCache.append(KUrl::fromLocalFile(file));
        }
    } else {
        // read skeleton files
        QDir skeletonRepository = QDir(Settings::courseRepositoryPath());
        skeletonRepository.setFilter(QDir::Files | QDir::Hidden);
        if (!skeletonRepository.cd("skeletons")) {
            kError() << "There is no subdirectory \"skeletons\" in directory " << skeletonRepository.path()
                << " cannot load skeletons.";
        } else {
            // read skeletons
            QFileInfoList list = skeletonRepository.entryInfoList();
            for (int i = 0; i < list.size(); ++i) {
                QFileInfo fileInfo = list.at(i);
                m_skeletonFileCache.append(KUrl::fromLocalFile(fileInfo.absoluteFilePath()));
            }
        }

        // read course files
        QDir courseRepository = QDir(Settings::courseRepositoryPath());
        if (!courseRepository.cd("courses")) {
            kError() << "There is no subdirectory \"courses\" in directory " << courseRepository.path()
                << " cannot load courses.";
        } else {
            // find courses
            courseRepository.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
            QFileInfoList courseDirList = courseRepository.entryInfoList();

            // traverse all course directories
            foreach (const QFileInfo &info, courseDirList) {
                QDir courseDir = QDir(info.absoluteFilePath());
                courseDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
                QFileInfoList courseLangDirList = courseDir.entryInfoList();

                // traverse all language directories for each course
                foreach (const QFileInfo &langInfo, courseLangDirList) {
                    QDir courseLangDir = QDir(langInfo.absoluteFilePath());
                    courseLangDir.setFilter(QDir::Files);
                    QStringList nameFilters;
                    nameFilters.append("*.xml");
                    QFileInfoList courses = courseLangDir.entryInfoList(nameFilters);

                    // find and add course files
                    foreach (const QFileInfo &courseInfo, courses) {
                        m_courseFileCache.append(courseInfo.filePath());
                    }
                }
            }
        } // done with reading courses
    }
}


void ResourceManager::loadResources()
{
    //TODO in the future loading should only be performed on request!
    //     this current implementation is extremely resource inefficient...

    // load resources
    foreach (const KUrl &file, m_languageFileCache) {
        loadLanguage(file);
    }
    foreach (const KUrl &file, m_courseFileCache) {
        addCourse(file);
    }
    foreach (const KUrl &file, m_skeletonFileCache) {
        addSkeleton(loadSkeleton(file));
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
    language->seti18nTitle(root.firstChildElement("i18nTitle").text());
    // create phoneme groups
    for (QDomElement groupNode = root.firstChildElement("phonemeGroups").firstChildElement();
         !groupNode.isNull();
         groupNode = groupNode.nextSiblingElement())
    {
        PhonemeGroup *group = language->addPhonemeGroup(
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
    course->setDescription(root.firstChildElement("description").text());
    if (!root.firstChildElement("foreignId").isNull()) {
        course->setForeignId(root.firstChildElement("foreignId").text());
    }

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
        unit->setId(unitNode.firstChildElement("id").text());
        unit->setCourse(course);
        unit->setTitle(unitNode.firstChildElement("title").text());
        if (!unitNode.firstChildElement("foreignId").isNull()) {
            unit->setForeignId(unitNode.firstChildElement("foreignId").text());
        }
        course->addUnit(unit);

        // create phrases
        for (QDomElement phraseNode = unitNode.firstChildElement("phrases").firstChildElement();
            !phraseNode.isNull();
            phraseNode = phraseNode.nextSiblingElement())
        {
            Phrase *phrase = new Phrase(unit);
            phrase->setId(phraseNode.firstChildElement("id").text());
            phrase->setText(phraseNode.firstChildElement("text").text());
            phrase->setUnit(unit);
            if (!phraseNode.firstChildElement("soundFile").text().isEmpty()) {
                phrase->setSound(KUrl::fromLocalFile(
                        courseFile.directory() + '/' + phraseNode.firstChildElement("soundFile").text())
                    );
            }
            phrase->setType(phraseNode.firstChildElement("type").text());
            if (!phraseNode.firstChildElement("foreignId").isNull()) {
                phrase->setForeignId(phraseNode.firstChildElement("foreignId").text());
            }

            // add phonemes
            QList<Phoneme *> phonemes = course->language()->phonemes();
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
    course->setModified(false);

    return course;
}

void ResourceManager::reloadCourseOrSkeleton(Course *courseOrSkeleton)
{
    if (!courseOrSkeleton) {
        kError() << "Cannot reload non-existing course";
        return;
    }
    if (!courseOrSkeleton->file().isValid()) {
        kError() << "Cannot reload temporary file, aborting.";
        return;
    }

    // figure out if this is a course or a skeleton
    if (courseOrSkeleton->language()) { // only course files have a language
        //TODO better add a check if this is contained in the course list
        // to catch possible errors
        KUrl file = courseOrSkeleton->file();
        removeCourse(courseOrSkeleton);
        addCourse(file);
    } else {
        KUrl file = courseOrSkeleton->file();
        Skeleton *skeleton = qobject_cast<Skeleton*>(courseOrSkeleton);
        removeSkeleton(skeleton);
        addSkeleton(loadSkeleton(file));
    }
}

void ResourceManager::updateCourseFromSkeleton(Course *course)
{
    //TODO implement status information that are shown at mainwindow
    if (course->foreignId().isEmpty())  {
        kError() << "No skeleton ID specified, aborting update.";
        return;
    }
    Course *skeleton = 0;
    QList<Skeleton *>::ConstIterator iter = m_skeletonList.constBegin();
    while (iter != m_skeletonList.constEnd()) {
        if ((*iter)->id() == course->foreignId()) {
            skeleton = *iter;
            break;
        }
        ++iter;
    }
    if (!skeleton)  {
        kError() << "Could not find skeleton with id " << course->foreignId() << ", aborting update.";
        return;
    }

    // update now
    foreach (Unit *unitSkeleton, skeleton->unitList()) {
        // import unit if not exists
        Unit *currentUnit = 0;
        bool found = false;
        foreach (Unit *unit, course->unitList()) {
            if (unit->foreignId() == unitSkeleton->id()) {
                found = true;
                currentUnit = unit;
                break;
            }
        }
        if (found == false) {
            currentUnit = new Unit(course);
            currentUnit->setId(QUuid::createUuid().toString());
            currentUnit->setTitle(unitSkeleton->title());
            currentUnit->setForeignId(unitSkeleton->id());
            currentUnit->setCourse(course);
            course->addUnit(currentUnit);
            course->setModified(true);
        }

        // update phrases
        foreach (Phrase *phraseSkeleton, unitSkeleton->phraseList()) {
            bool found = false;
            foreach (Phrase *phrase, currentUnit->phraseList()) {
                if (phrase->foreignId() == phraseSkeleton->id()) {
                    found = true;
                    break;
                }
            }
            if (found == false) {
                Phrase *newPhrase = new Phrase(course);
                newPhrase->setForeignId(phraseSkeleton->id());
                newPhrase->setId(QUuid::createUuid().toString());
                newPhrase->setText(phraseSkeleton->text());
                newPhrase->setType(phraseSkeleton->type());
                newPhrase->setUnit(currentUnit);
                currentUnit->addPhrase(newPhrase);
                course->setModified(true);
            }
        }
    }
    // FIXME deassociate removed phrases

    kDebug() << "Update performed!";
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

Skeleton * ResourceManager::loadSkeleton(const KUrl& skeletonFile)
{
    if (!skeletonFile.isLocalFile()) {
        kWarning() << "Cannot open skeleton file at " << skeletonFile.toLocalFile() << ", aborting.";
        return 0;
    }

    QXmlSchema schema = loadXmlSchema("skeleton");
    if (!schema.isValid()) {
        return 0;
    }

    QDomDocument document = loadDomDocument(skeletonFile, schema);
    if (document.isNull()) {
        kWarning() << "Could not parse document " << skeletonFile.toLocalFile() << ", aborting.";
        return 0;
    }

    // create skeleton
    QDomElement root(document.documentElement());
    Skeleton *skeleton = new Skeleton(this);

    skeleton->setFile(skeletonFile);
    skeleton->setId(root.firstChildElement("id").text());
    skeleton->setTitle(root.firstChildElement("title").text());
    skeleton->setDescription(root.firstChildElement("title").text());

    // create units
    for (QDomElement unitNode = root.firstChildElement("units").firstChildElement();
         !unitNode.isNull();
         unitNode = unitNode.nextSiblingElement())
    {
        Unit *unit = new Unit(skeleton);
        unit->setId(unitNode.firstChildElement("id").text());
        unit->setCourse(skeleton);
        unit->setTitle(unitNode.firstChildElement("title").text());
        skeleton->addUnit(unit);

        // create phrases
        for (QDomElement phraseNode = unitNode.firstChildElement("phrases").firstChildElement();
            !phraseNode.isNull();
            phraseNode = phraseNode.nextSiblingElement())
        {
            Phrase *phrase = new Phrase(unit);
            phrase->setId(phraseNode.firstChildElement("id").text());
            phrase->setText(phraseNode.firstChildElement("text").text());
            phrase->setType(phraseNode.firstChildElement("type").text());
            phrase->setUnit(unit);

            unit->addPhrase(phrase);
        }
    }
    skeleton->setModified(false);

    return skeleton;
}

void ResourceManager::addSkeleton(Skeleton* skeleton)
{
    emit skeletonAboutToBeAdded(skeleton, m_skeletonList.count());
    m_skeletonList.append(skeleton);
    emit skeletonAdded();
}

void ResourceManager::removeSkeleton(Skeleton *skeleton)
{
    int index = m_skeletonList.indexOf(skeleton);
    emit skeletonAboutToBeRemoved(index, index);
    m_skeletonList.removeAt(index);
    emit skeletonRemoved();
    skeleton->deleteLater();
}

QList< Skeleton* > ResourceManager::skeletonList() const
{
    return m_skeletonList;
}

void ResourceManager::syncCourse(Course *course)
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
            QDomElement phrasePhonemeListElement = document.createElement("phonemes");

            phraseIdElement.appendChild(document.createTextNode(phrase->id()));
            phraseTextElement.appendChild(document.createTextNode(phrase->text()));
            phraseSoundFileElement.appendChild(document.createTextNode(phrase->sound().toLocalFile()));
            phraseTypeElement.appendChild(document.createTextNode(phrase->typeString()));

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
            phraseElement.appendChild(phraseSoundFileElement);
            phraseElement.appendChild(phraseTypeElement);
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
    if (!course->foreignId().isEmpty()) {
        QDomElement courseForeignIdElement = document.createElement("foreignId");
        courseForeignIdElement.appendChild(document.createTextNode(course->foreignId()));
        root.appendChild(courseForeignIdElement);
    }
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

void ResourceManager::syncSkeleton(Skeleton *skeleton)
{
    Q_ASSERT(skeleton->file().isValid());
    Q_ASSERT(skeleton->file().isLocalFile());
    Q_ASSERT(!skeleton->file().isEmpty());


    QDomDocument document;
    // prepare xml header
    QDomProcessingInstruction header = document.createProcessingInstruction("xml", "version=\"1.0\"");
    document.appendChild(header);

    // create main element
    QDomElement root = document.createElement("skeleton");
    document.appendChild(root);

    QDomElement idElement = document.createElement("id");
    QDomElement titleElement = document.createElement("title");
    QDomElement descriptionElement = document.createElement("description");

    idElement.appendChild(document.createTextNode(skeleton->id()));
    titleElement.appendChild(document.createTextNode(skeleton->title()));
    descriptionElement.appendChild(document.createTextNode(skeleton->description()));

    QDomElement unitListElement = document.createElement("units");
    // create units
    foreach (Unit *unit, skeleton->unitList()) {
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
            QDomElement phraseTypeElement = document.createElement("type");

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
    QFile file;
    file.setFileName(skeleton->file().toLocalFile());
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
        kWarning() << "Schema is not valid, aborting loading of XML document:" << path.toLocalFile();
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
