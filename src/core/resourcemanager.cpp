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

#include "resourcemanager.h"
#include "language.h"
#include "course.h"
#include "skeleton.h"
#include "unit.h"
#include "phrase.h"
#include "phoneme.h"
#include "phonemegroup.h"
#include "resources/languageresource.h"
#include "resources/courseresource.h"
#include "resources/skeletonresource.h"
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
    , m_loadingSkeletons(false)
{
    updateResourceFileCache();
}

void ResourceManager::updateResourceFileCache()
{
    // find all course and skeleton files and cache paths to them
    if (!Settings::useCourseRepository()) {
        QStringList courseFiles = KGlobal::dirs()->findAllResources("data",QString("artikulate/courses/*/*/*.xml"));
        foreach (const QString &file, courseFiles) {
            KUrl courseFile = KUrl::fromLocalFile(file);
            // get directory name, which is the language identifier for this course
            // TODO allow usage of non-language ID named course folders
            QString directory = courseFile.directory().section('/', -1);
            m_courseFileCache.insert(directory, courseFile);
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
                    QString languageId = langInfo.fileName();
                    QDir courseLangDir = QDir(langInfo.absoluteFilePath());
                    courseLangDir.setFilter(QDir::Files);
                    QStringList nameFilters;
                    nameFilters.append("*.xml");
                    QFileInfoList courses = courseLangDir.entryInfoList(nameFilters);

                    // find and add course files
                    foreach (const QFileInfo &courseInfo, courses) {
                        m_courseFileCache.insert(languageId, courseInfo.filePath());
                    }
                }
            }
        } // done with reading courses
    }
}

bool ResourceManager::isRepositoryManager() const
{
    return Settings::useCourseRepository() && !Settings::courseRepositoryPath().isEmpty();
}

void ResourceManager::loadResources()
{
    // load language resources
    // all other resources are only loaded on demand
    QStringList languageFiles = KGlobal::dirs()->findAllResources("appdata",QString("languages/*.xml"));
    foreach (const QString &file, languageFiles) {
        LanguageResource *resource = new LanguageResource(this, KUrl::fromLocalFile(file));
        Language *language = resource->language();

        emit languageAboutToBeAdded(language, m_languageResources.count());
        m_languageResources.append(resource);
        emit languageAdded();
    }
}

QList< LanguageResource* > ResourceManager::languageResources() const
{
    return m_languageResources;
}

Language * ResourceManager::language(int index) const
{
    Q_ASSERT (index >= 0 && index < m_languageResources.count());
    return m_languageResources.at(index)->language();
}

QList< CourseResource* > ResourceManager::courseResources(Language *language)
{
    //TODO remove this caching, not neededn anymore
    if (!m_courseResources.contains(language->id()) && m_courseFileCache.contains(language->id())) {
        m_courseResources.insert(language->id(), QList< CourseResource* >());
        QList<KUrl> courseFiles = m_courseFileCache.values(language->id());
        for (int i = 0; i < courseFiles.size(); ++i) {
            addCourse(courseFiles[i]);
        }
    }

    // return empty list if no course available
    if (!m_courseResources.contains(language->id())) {
        return QList< CourseResource* >();
    }
    return m_courseResources[language->id()];
}

Course * ResourceManager::course(Language *language, int index) const
{
    Q_ASSERT(m_courseResources.contains(language->id()));
    Q_ASSERT(index >= 0 && index < m_courseResources[language->id()].count());

    return m_courseResources[language->id()].at(index)->course();
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
        foreach (SkeletonResource *resource, m_skeletonList) {
            if (resource->identifier() == courseOrSkeleton->id()) {
                resource->reload();
                return;
            }
        }
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
    QList<SkeletonResource *>::ConstIterator iter = m_skeletonList.constBegin();
    while (iter != m_skeletonList.constEnd()) {
        if ((*iter)->identifier() == course->foreignId()) {
            skeleton = (*iter)->skeleton();
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
                    if (phrase->i18nText() != phraseSkeleton->text()) {
                        phrase->setEditState(Phrase::Unknown);
                        phrase->seti18nText(phraseSkeleton->text());
                    }
                    found = true;
                    break;
                }
            }
            if (found == false) {
                Phrase *newPhrase = new Phrase(course);
                newPhrase->setForeignId(phraseSkeleton->id());
                newPhrase->setId(QUuid::createUuid().toString());
                newPhrase->setText(phraseSkeleton->text());
                newPhrase->seti18nText(phraseSkeleton->text());
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
    CourseResource *resource = new CourseResource(this, courseFile);
    if (resource->language().isEmpty()) {
        kError() << "Could not load course, language unknown:" << courseFile.toLocalFile();
        return false;
    }
    addCourseResource(resource);

    return true;
}

void ResourceManager::addCourseResource(CourseResource *resource)
{
    Q_ASSERT(m_courseResources.contains(resource->language()));

    if (m_courseResources.contains(resource->language())) {
        emit courseAboutToBeAdded(resource->course(), m_courseResources[resource->language()].count());
    }
    else {
        emit courseAboutToBeAdded(resource->course(), 0);
        m_courseResources.insert(resource->language(), QList<CourseResource*>());
    }
    m_courseResources[resource->language()].append(resource);
    emit courseAdded();
}

void ResourceManager::removeCourse(Course *course)
{
    for (int index=0; index < m_courseResources[course->language()->id()].length(); index++) {
        if (m_courseResources[course->language()->id()].at(index)->course() == course) {
            emit courseAboutToBeRemoved(index, index);
            m_courseResources[course->language()->id()].removeAt(index);
            emit courseRemoved();
            course->deleteLater();
            return;
        }
    }
}

void ResourceManager::newCourseDialog()
{
    QPointer<NewCourseDialog> dialog = new NewCourseDialog(this);
    if (dialog->exec() == QDialog::Accepted ) {
        addCourseResource(dialog->courseResource());
    }
}

void ResourceManager::addSkeleton(const KUrl &skeletonFile)
{
    SkeletonResource *resource = new SkeletonResource(this, skeletonFile);
    addSkeletonResource(resource);
}

void ResourceManager::addSkeletonResource(SkeletonResource* resource)
{
    emit skeletonAboutToBeAdded(resource->skeleton(), m_skeletonList.count());
    m_skeletonList.append(resource);
    emit skeletonAdded();
}

void ResourceManager::removeSkeleton(Skeleton *skeleton)
{
    for (int index=0; index < m_skeletonList.length(); ++index) {
        if (m_skeletonList.at(index)->identifier() == skeleton->id()) {
            emit skeletonAboutToBeRemoved(index, index);
            m_skeletonList.removeAt(index);
            emit skeletonRemoved();
            skeleton->deleteLater();
            return;
        }
    }
}

QList< SkeletonResource* > ResourceManager::skeletonResources()
{
    //TODO compare with cache if content change and possibly update
    if (m_skeletonList.isEmpty() && !m_skeletonFileCache.isEmpty() && !m_loadingSkeletons) {
        m_loadingSkeletons = true;
        foreach(const KUrl &path, m_skeletonFileCache) {
            addSkeleton(path);
        }
    }
    m_loadingSkeletons = false;

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
