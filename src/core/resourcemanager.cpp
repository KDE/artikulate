/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
#include "settings.h"
#include "liblearnerprofile/src/profilemanager.h"
#include "liblearnerprofile/src/learninggoal.h"

#include <QIODevice>
#include <QFile>
#include <QPointer>
#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QDomDocument>
#include <QUuid>
#include <QDir>
#include <QDirIterator>
#include "artikulate_debug.h"
#include <QUrl>
#include <QStandardPaths>

ResourceManager::ResourceManager(QObject *parent)
    : QObject(parent)
{
}

void ResourceManager::loadCourseResources()
{
    // reload config, could be changed in dialogs
    Settings::self()->load();

    // register skeleton resources
    QDir skeletonRepository = QDir(Settings::courseRepositoryPath());
    skeletonRepository.setFilter(QDir::Files | QDir::Hidden);
    if (!skeletonRepository.cd("skeletons")) {
        qCritical() << "There is no subdirectory \"skeletons\" in directory " << skeletonRepository.path()
            << " cannot load skeletons.";
    } else {
        // read skeletons
        QFileInfoList list = skeletonRepository.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            addSkeleton(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
        }
    }

    // register contributor course files
    QDir courseRepository = QDir(Settings::courseRepositoryPath());
    if (!courseRepository.cd("courses")) {
        qCritical() << "There is no subdirectory \"courses\" in directory " << courseRepository.path()
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
                    CourseResource * course = addCourse(QUrl::fromLocalFile(courseInfo.filePath()));
                    if (course != nullptr) {
                        course->setContributorResource(true);
                    }
                }
            }
        }
    }

    // register GHNS course resources
    QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::DataLocation);
    foreach (const QString &testdir, dirs) {
        QDirIterator it(testdir + "/courses/", QDirIterator::Subdirectories);
        while (it.hasNext()) {
            QDir dir(it.next());
            dir.setFilter(QDir::Files | QDir::NoSymLinks);
            QFileInfoList list = dir.entryInfoList();
            for (int i = 0; i < list.size(); ++i) {
                QFileInfo fileInfo = list.at(i);
                if (fileInfo.completeSuffix() != "xml") {
                    continue;
                }
                addCourse(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
            }
        }
    }

    //TODO this signal should only be emitted when repository was added/removed
    // yet the call to this method is very seldom and emitting it too often is not that harmful
    emit repositoryChanged();
}

void ResourceManager::loadLanguageResources()
{
    // load language resources
    // all other resources are only loaded on demand
    QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);
    foreach (const QString &testdir, dirs) {
        QDir dir(testdir + "/artikulate/languages/");
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList list = dir.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            if (fileInfo.completeSuffix() != "xml") {
                continue;
            }
            addLanguage(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
        }
    }
}

void ResourceManager::sync()
{
    QMap< QString, QList< CourseResource* > >::iterator iter;
    for (iter = m_courseResources.begin(); iter != m_courseResources.end(); ++iter) {
        foreach (auto const &courseRes, iter.value()) {
            courseRes->sync();
        }
    }
    foreach (auto const &courseRes, m_skeletonResources) {
        courseRes->sync();
    }
}

bool ResourceManager::modified() const
{
    QMap< QString, QList< CourseResource* > >::const_iterator iter;
    for (iter = m_courseResources.constBegin(); iter != m_courseResources.constEnd(); ++iter) {
        foreach (auto const &courseRes, iter.value()) {
            if (courseRes->isOpen() && courseRes->course()->modified()) {
                return true;
            }
        }
    }
    foreach (auto const &courseRes, m_skeletonResources) {
        if (courseRes->isOpen() && courseRes->skeleton()->modified()) {
            return true;
        }
    }
    return false;
}

void ResourceManager::registerLearningGoals(LearnerProfile::ProfileManager *profileManger)
{
    foreach (LanguageResource *languageResource, languageResources()) {
        profileManger->registerGoal(
            LearnerProfile::LearningGoal::Language,
            languageResource->language()->id(),
            languageResource->language()->i18nTitle()
            );
    }
}

void ResourceManager::addLanguage(const QUrl &languageFile)
{
    if (m_loadedResources.contains(languageFile.toLocalFile())) {
        return;
    }

    LanguageResource *resource = new LanguageResource(this, languageFile);

    emit languageResourceAboutToBeAdded(resource, m_languageResources.count());
    m_languageResources.append(resource);
    m_loadedResources.append(languageFile.toLocalFile());
    m_courseResources.insert(resource->identifier(), QList<CourseResource*>());
    emit languageResourceAdded();
}

bool ResourceManager::isRepositoryManager() const
{
    return !Settings::courseRepositoryPath().isEmpty();
}

QString ResourceManager::repositoryUrl() const
{
    return Settings::courseRepositoryPath();
}

QList< LanguageResource* > ResourceManager::languageResources() const
{
    return m_languageResources;
}

Language * ResourceManager::language(int index) const
{
    Q_ASSERT(index >= 0 && index < m_languageResources.count());
    return m_languageResources.at(index)->language();
}

Language * ResourceManager::language(LearnerProfile::LearningGoal *learningGoal) const
{
    if (!learningGoal) {
        return nullptr;
    }
    if (learningGoal->category() != LearnerProfile::LearningGoal::Language) {
        qCritical() << "Cannot translate non-language learning goal to language";
        return nullptr;
    }
    foreach (LanguageResource *resource, m_languageResources) {
        if (resource->identifier() == learningGoal->identifier()) {
            return resource->language();
        }
    }
    qCritical() << "No language registered with identifier " << learningGoal->identifier() << ": aborting";
    return nullptr;
}

QList< CourseResource* > ResourceManager::courseResources(Language *language)
{
    Q_ASSERT(language);
    if (!language) {
        return QList< CourseResource* >();
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
        qCritical() << "Cannot reload non-existing course";
        return;
    }
    if (!courseOrSkeleton->file().isValid()) {
        qCritical() << "Cannot reload temporary file, aborting.";
        return;
    }

    // figure out if this is a course or a skeleton
    if (courseOrSkeleton->language()) { // only course files have a language
        //TODO better add a check if this is contained in the course list
        // to catch possible errors
        QUrl file = courseOrSkeleton->file();
        m_loadedResources.removeOne(courseOrSkeleton->file().toLocalFile());
        removeCourse(courseOrSkeleton);
        addCourse(file);
    } else {
        foreach (SkeletonResource *resource, m_skeletonResources) {
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
        qCritical() << "No skeleton ID specified, aborting update.";
        return;
    }
    Course *skeleton = nullptr;
    QList<SkeletonResource *>::ConstIterator iter = m_skeletonResources.constBegin();
    while (iter != m_skeletonResources.constEnd()) {
        if ((*iter)->identifier() == course->foreignId()) {
            skeleton = (*iter)->skeleton();
            break;
        }
        ++iter;
    }
    if (!skeleton)  {
        qCritical() << "Could not find skeleton with id " << course->foreignId() << ", aborting update.";
        return;
    }

    // update now
    foreach (Unit *unitSkeleton, skeleton->unitList()) {
        // import unit if not exists
        Unit *currentUnit = nullptr;
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

    qCDebug(ARTIKULATE_LOG) << "Update performed!";
}

CourseResource * ResourceManager::addCourse(const QUrl &courseFile)
{
    CourseResource *resource = new CourseResource(this, courseFile);
    if (resource->language().isEmpty()) {
        qCritical() << "Could not load course, language unknown:" << courseFile.toLocalFile();
        return nullptr;
    }

    // skip already loaded resources
    if (m_loadedResources.contains(courseFile.toLocalFile())) {
        return nullptr;
    }
    m_loadedResources.append(courseFile.toLocalFile());
    addCourseResource(resource);
    emit languageCoursesChanged();
    return resource;
}

void ResourceManager::addCourseResource(CourseResource *resource)
{
    Q_ASSERT(m_courseResources.contains(resource->language()));

    if (m_courseResources.contains(resource->language())) {
        emit courseResourceAboutToBeAdded(resource, m_courseResources[resource->language()].count());
    }
    else {
        emit courseResourceAboutToBeAdded(resource, 0);
        m_courseResources.insert(resource->language(), QList<CourseResource*>());
    }
    m_courseResources[resource->language()].append(resource);
    emit courseResourceAdded();
}

void ResourceManager::removeCourse(Course *course)
{
    for (int index = 0; index < m_courseResources[course->language()->id()].length(); ++index) {
        if (m_courseResources[course->language()->id()].at(index)->course() == course) {
            emit courseResourceAboutToBeRemoved(index);
            m_courseResources[course->language()->id()].removeAt(index);
            course->deleteLater();
            return;
        }
    }
}

Course * ResourceManager::createCourse(Language *language, Skeleton *skeleton)
{
    // set path
    QString path = QString("%1/%2/%3/%4/%4.xml")
        .arg(Settings::courseRepositoryPath())
        .arg("courses")
        .arg(skeleton->id())
        .arg(language->id());

    CourseResource * courseRes = new CourseResource(this, QUrl::fromLocalFile(path));
    Q_ASSERT(courseRes);

    Course *course = courseRes->course();
    Q_ASSERT(course);
    course->setId(QUuid::createUuid().toString());
    course->setTitle(skeleton->title());
    course->setDescription(skeleton->description());
    course->setFile(QUrl::fromLocalFile(path));
    course->setLanguage(language);

    // set skeleton
    course->setForeignId(skeleton->id());

    addCourseResource(courseRes);

    return course;
}

void ResourceManager::addSkeleton(const QUrl &skeletonFile)
{
    SkeletonResource *resource = new SkeletonResource(this, skeletonFile);
    addSkeletonResource(resource);
}

void ResourceManager::addSkeletonResource(SkeletonResource *resource)
{
    // skip already loaded resources
    if (m_loadedResources.contains(resource->path().toLocalFile())) {
        return;
    }
    m_loadedResources.append(resource->path().toLocalFile());
    emit skeletonAboutToBeAdded(resource->skeleton(), m_skeletonResources.count());
    m_skeletonResources.append(resource);
    emit skeletonAdded();
}

void ResourceManager::removeSkeleton(Skeleton *skeleton)
{
    for (int index = 0; index < m_skeletonResources.length(); ++index) {
        if (m_skeletonResources.at(index)->identifier() == skeleton->id()) {
            emit skeletonAboutToBeRemoved(index, index);
            m_skeletonResources.removeAt(index);
            emit skeletonRemoved();
            skeleton->deleteLater();
            return;
        }
    }
}

QList< SkeletonResource* > ResourceManager::skeletonResources()
{
    return m_skeletonResources;
}
