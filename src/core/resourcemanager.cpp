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

#include <KGlobal>
#include <QDebug>
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
                    CourseResource * course = addCourse(courseInfo.filePath());
                    if (course != 0) {
                        course->setContributorResource(true);
                    }
                }
            }
        }
    }

    // register GHNS course resources
    QStringList courseFiles = QStandardPaths::locateAll(QStandardPaths::GenericDataLocation, QString("artikulate/courses/*/*/*.xml"));
    foreach (const QString &file, courseFiles) {
        QUrl courseFile = QUrl::fromLocalFile(file);
        // get directory name, which is the language identifier for this course
        // TODO allow usage of non-language ID named course folders
        QString directory = courseFile.adjusted(QUrl::RemoveFilename|QUrl::StripTrailingSlash).path().section('/', -1);
        addCourse(courseFile);
    }

    //TODO this signal should only be emitted when repository was added/removed
    // yet the call to this method is very seldom and emitting it too often is not that harmful
    emit repositoryChanged();
}

void ResourceManager::loadLanguageResources()
{
    // load language resources
    // all other resources are only loaded on demand
    QStringList languageFiles = QStandardPaths::locateAll(QStandardPaths::DataLocation, QString("languages/*.xml"));
    foreach (const QString &file, languageFiles) {
        addLanguage(QUrl::fromLocalFile(file));
    }
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
        return 0;
    }
    if (learningGoal->category() != LearnerProfile::LearningGoal::Language) {
        qCritical() << "Cannot translate non-language learning goal to language";
        return 0;
    }
    foreach (LanguageResource *resource, m_languageResources) {
        if (resource->identifier() == learningGoal->identifier()) {
            return resource->language();
        }
    }
    qCritical() << "No language registered with identifier " << learningGoal->identifier() << ": aborting";
    return 0;
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
    Course *skeleton = 0;
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

    qDebug() << "Update performed!";
}

CourseResource * ResourceManager::addCourse(const QUrl &courseFile)
{
    CourseResource *resource = new CourseResource(this, courseFile);
    if (resource->language().isEmpty()) {
        qCritical() << "Could not load course, language unknown:" << courseFile.toLocalFile();
        return 0;
    }

    // skip already loaded resources
    if (m_loadedResources.contains(courseFile.toLocalFile())) {
        return 0;
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
    for (int index=0; index < m_courseResources[course->language()->id()].length(); index++) {
        if (m_courseResources[course->language()->id()].at(index)->course() == course) {
            emit courseResourceAboutToBeRemoved(index);
            m_courseResources[course->language()->id()].removeAt(index);
            course->deleteLater();
            return;
        }
    }
}

void ResourceManager::newCourseDialog(Language *language)
{
    QPointer<NewCourseDialog> dialog = new NewCourseDialog(this);
    if (language != 0) {
        dialog->setLanguage(language);
    }
    if (dialog->exec() == QDialog::Accepted) {
        CourseResource *tempCourse = dialog->courseResource();
        tempCourse->setContributorResource(true);
        addCourseResource(dialog->courseResource());
        emit languageCoursesChanged();
    }
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
    for (int index=0; index < m_skeletonResources.length(); ++index) {
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
