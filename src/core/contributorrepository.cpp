/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "contributorrepository.h"
#include "artikulate_debug.h"
#include "language.h"
#include "skeleton.h"
#include "unit.h"
#include "phrase.h"
#include "phoneme.h"
#include "phonemegroup.h"
#include "resources/languageresource.h"
#include "resources/editablecourseresource.h"
#include "resources/skeletonresource.h"
#include "liblearnerprofile/src/profilemanager.h"
#include "liblearnerprofile/src/learninggoal.h"

#include <QFile>
#include <QUuid>
#include <QDir>
#include <QDirIterator>
#include <QUrl>
#include <QStandardPaths>

ContributorRepository::ContributorRepository(QObject *parent)
    : IResourceRepository()
{
    loadLanguageResources();
}

void ContributorRepository::loadLanguageResources()
{
    // load language resources
    // all other resources are only loaded on demand
    QDir dir(":/artikulate/languages/");
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.completeSuffix() != QLatin1String("xml")) {
            continue;
        }
        addLanguage(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
    }
}

void ContributorRepository::sync()
{
//    QMap< QString, QList< CourseResource* > >::iterator iter;
//    for (iter = m_courseResources.begin(); iter != m_courseResources.end(); ++iter) {
//        foreach (auto const &courseRes, iter.value()) {
//            courseRes->sync();
//        }
//    }
//    foreach (auto const &courseRes, m_skeletonResources) {
//        courseRes->sync();
//    }
}

bool ContributorRepository::modified() const
{
    for (auto iter = m_courses.constBegin(); iter != m_courses.constEnd(); ++iter) {
        for (auto *course : iter.value()) {
            if (course->isModified()) {
                return true;
            }
        }
    }
    foreach (auto const &courseRes, m_skeletonResources) {
        if (courseRes->isOpen() && courseRes->skeleton()->isModified()) {
            return true;
        }
    }
    return false;
}

void ContributorRepository::addLanguage(const QUrl &languageFile)
{
    if (m_loadedResources.contains(languageFile.toLocalFile())) {
        return;
    }

    LanguageResource *resource = new LanguageResource(languageFile);

    emit languageResourceAboutToBeAdded(resource, m_languageResources.count());
    m_languageResources.append(resource);
    m_loadedResources.append(languageFile.toLocalFile());
    m_courses.insert(resource->identifier(), QList<EditableCourseResource*>());
    emit languageResourceAdded();
}

QString ContributorRepository::storageLocation() const
{
    return m_storageLocation;
}

void ContributorRepository::setStorageLocation(const QString &path)
{
    m_storageLocation = path;
}

QList< LanguageResource* > ContributorRepository::languageResources() const
{
    return m_languageResources;
}

QVector<Language *> ContributorRepository::languages() const
{
    QVector<Language *> languages;
    for (auto resourse : m_languageResources) {
        languages.append(resourse->language());
    }
    return languages;
}

Language * ContributorRepository::language(int index) const
{
    Q_ASSERT(index >= 0 && index < m_languageResources.count());
    return m_languageResources.at(index)->language();
}

Language * ContributorRepository::language(LearnerProfile::LearningGoal *learningGoal) const
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

QList<EditableCourseResource *> ContributorRepository::courseResources(Language *language)
{
    if (!language) {
        QList<EditableCourseResource *> courses;
        for (auto iter = m_courses.constBegin(); iter != m_courses.constEnd(); ++iter) {
            courses.append(iter.value());
        }
        return courses;
    }
    // return empty list if no course available for language
    if (!m_courses.contains(language->id())) {
        return QList< EditableCourseResource* >();
    }
    return m_courses[language->id()];
}

QVector<ICourse *> ContributorRepository::courses() const
{
    QVector<ICourse *> courses;
    for (const auto &courseList : m_courses) {
        for (const auto &course : courseList) {
            courses.append(course);
        }
    }
    return courses;
}

QVector<ICourse *> ContributorRepository::courses(Language *language) const
{
    if (language == nullptr) {
        return courses();
    }

    QVector<ICourse *> courses;
    if (m_courses.contains(language->id())) {
        for (const auto &course : m_courses[language->id()]) {
            courses.append(course);
        }
    }
    return courses;
}

EditableCourseResource * ContributorRepository::course(Language *language, int index) const
{
    Q_ASSERT(m_courses.contains(language->id()));
    Q_ASSERT(index >= 0 && index < m_courses[language->id()].count());

    return m_courses[language->id()].at(index);
}

void ContributorRepository::reloadCourseOrSkeleton(ICourse *courseOrSkeleton)
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

void ContributorRepository::reloadCourses()
{
    // register skeleton resources
    QDir skeletonDirectory = QDir(storageLocation());

    skeletonDirectory.setFilter(QDir::Files | QDir::Hidden);
    if (!skeletonDirectory.cd(QStringLiteral("skeletons"))) {
        qCritical() << "There is no subdirectory \"skeletons\" in directory " << skeletonDirectory.path()
            << " cannot load skeletons.";
    } else {
        // read skeletons
        QFileInfoList list = skeletonDirectory.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            addSkeleton(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
        }
    }

    // register contributor course files
    QDir courseDirectory(storageLocation());
    if (!courseDirectory.cd(QStringLiteral("courses"))) {
        qCritical() << "There is no subdirectory \"courses\" in directory " << courseDirectory.path()
            << " cannot load courses.";
    } else {
        // find courses
        courseDirectory.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
        QFileInfoList courseDirList = courseDirectory.entryInfoList();

        // traverse all course directories
        for (const QFileInfo &info : courseDirList) {
            QDir courseDir = QDir(info.absoluteFilePath());
            courseDir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
            QFileInfoList courseLangDirList = courseDir.entryInfoList();

            // traverse all language directories for each course
            for (const QFileInfo &langInfo : courseLangDirList) {
                QDir courseLangDir = QDir(langInfo.absoluteFilePath());
                courseLangDir.setFilter(QDir::Files);
                QStringList nameFilters;
                nameFilters.append(QStringLiteral("*.xml"));
                QFileInfoList courses = courseLangDir.entryInfoList(nameFilters);

                // find and add course files
                for (const QFileInfo &courseInfo : courses) {
                    addCourse(QUrl::fromLocalFile(courseInfo.filePath()));
                }
            }
        }
    }
    //TODO this signal should only be emitted when repository was added/removed
    // yet the call to this method is very seldom and emitting it too often is not that harmful
    emit repositoryChanged();
}

void ContributorRepository::updateCourseFromSkeleton(EditableCourseResource *course)
{
    //TODO implement status information that are shown at mainwindow
    if (course->foreignId().isEmpty())  {
        qCritical() << "No skeleton ID specified, aborting update.";
        return;
    }
    ICourse *skeleton = nullptr;
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

EditableCourseResource * ContributorRepository::addCourse(const QUrl &courseFile)
{
    EditableCourseResource *resource = new EditableCourseResource(courseFile, this);
    if (resource->language() == nullptr) {
        delete resource;
        qCritical() << "Could not load course, language unknown:" << courseFile.toLocalFile();
        return nullptr;
    }

    // skip already loaded resources
    if (m_loadedResources.contains(courseFile.toLocalFile())) {
        delete resource;
        return nullptr;
    }
    m_loadedResources.append(courseFile.toLocalFile());
    addCourseResource(resource);
    emit languageCoursesChanged();
    return resource;
}

void ContributorRepository::addCourseResource(EditableCourseResource *resource)
{
    Q_ASSERT(m_courses.contains(resource->language()->id()));

    if (!m_courses.contains(resource->language()->id())) {
        m_courses.insert(resource->language()->id(), QList<EditableCourseResource*>());
    }
    emit courseAboutToBeAdded(resource, m_courses[resource->language()->id()].count());
    m_courses[resource->language()->id()].append(resource);
    emit courseAdded();
}

void ContributorRepository::removeCourse(ICourse *course)
{
    for (int index = 0; index < m_courses[course->language()->id()].length(); ++index) {
        if (m_courses[course->language()->id()].at(index) == course) {
            emit courseAboutToBeRemoved(index);
            m_courses[course->language()->id()].removeAt(index);
            emit courseRemoved();
            course->deleteLater();
            return;
        }
    }
}

EditableCourseResource * ContributorRepository::createCourse(Language *language, Skeleton *skeleton)
{
    // set path
    QString path = QStringLiteral("%1/%2/%3/%4/%4.xml")
        .arg(storageLocation(),
             QStringLiteral("courses"),
             skeleton->id(),
             language->id());

    EditableCourseResource * course = new EditableCourseResource(QUrl::fromLocalFile(path), this);

    Q_ASSERT(course);
    course->setId(QUuid::createUuid().toString());
    course->setTitle(skeleton->title());
    course->setDescription(skeleton->description());
    course->setFile(QUrl::fromLocalFile(path));
    course->setLanguage(language);

    // set skeleton
    course->setForeignId(skeleton->id());

    addCourseResource(course);

    return course;
}

void ContributorRepository::addSkeleton(const QUrl &skeletonFile)
{
    SkeletonResource *resource = new SkeletonResource(skeletonFile);
    addSkeletonResource(resource);
}

void ContributorRepository::addSkeletonResource(SkeletonResource *resource)
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

void ContributorRepository::removeSkeleton(Skeleton *skeleton)
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

QList< SkeletonResource* > ContributorRepository::skeletonResources()
{
    return m_skeletonResources;
}
