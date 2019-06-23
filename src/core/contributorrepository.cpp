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

ContributorRepository::ContributorRepository()
    : IEditableRepository()
{
    loadLanguageResources();
}

ContributorRepository::~ContributorRepository()
{
    for (auto skeleton : m_skeletonResources) {
        skeleton->deleteLater();
    }
    m_skeletonResources.clear();
    for (auto language : m_languageResources) {
        language->deleteLater();
    }
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
        for (auto course : iter.value()) {
            if (course->isModified()) {
                return true;
            }
        }
    }
    for (auto const &courseRes : m_skeletonResources) {
        if (courseRes->isModified()) {
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

    std::shared_ptr<LanguageResource> resource(new LanguageResource(languageFile));

    emit languageResourceAboutToBeAdded(resource.get(), m_languageResources.count());
    m_languageResources.append(resource);
    m_loadedResources.append(languageFile.toLocalFile());
    m_courses.insert(resource->identifier(), QVector<std::shared_ptr<EditableCourseResource>>());
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

QVector<std::shared_ptr<Language>> ContributorRepository::languages() const
{
    QVector<std::shared_ptr<Language>> languages;
    for (auto resourse : m_languageResources) {
        languages.append(resourse->language());
    }
    return languages;
}

std::shared_ptr<Language> ContributorRepository::language(int index) const
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
    for (auto resource : m_languageResources) {
        if (resource->identifier() == learningGoal->identifier()) {
            return resource->language().get();
        }
    }
    qCritical() << "No language registered with identifier " << learningGoal->identifier() << ": aborting";
    return nullptr;
}

QVector<std::shared_ptr<EditableCourseResource>> ContributorRepository::courseResources(std::shared_ptr<Language> language)
{
    if (!language) {
        QVector<std::shared_ptr<EditableCourseResource>> courses;
        for (auto iter = m_courses.constBegin(); iter != m_courses.constEnd(); ++iter) {
            courses.append(iter.value());
        }
        return courses;
    }
    // return empty list if no course available for language
    if (!m_courses.contains(language->id())) {
        return QVector<std::shared_ptr<EditableCourseResource>>();
    }
    return m_courses[language->id()];
}

QVector<std::shared_ptr<ICourse>> ContributorRepository::courses() const
{
    QVector<std::shared_ptr<ICourse>> courses;
    for (const auto &courseList : m_courses) {
        for (const auto &course : courseList) {
            courses.append(course);
        }
    }
    return courses;
}

QVector<std::shared_ptr<IEditableCourse>> ContributorRepository::editableCourses() const
{
    QVector<std::shared_ptr<IEditableCourse>> courses;
    for (const auto &courseList : m_courses) {
        for (const auto &course : courseList) {
            courses.append(course);
        }
    }
    return courses;
}

QVector<std::shared_ptr<ICourse>> ContributorRepository::courses(const QString &languageId) const
{
    if (languageId.isEmpty()) {
        return courses();
    }

    QVector<std::shared_ptr<ICourse>> courses;
    if (m_courses.contains(languageId)) {
        for (const auto &course : m_courses[languageId]) {
            courses.append(course);
        }
    }
    return courses;
}

std::shared_ptr<IEditableCourse> ContributorRepository::editableCourse(std::shared_ptr<Language> language, int index) const
{
    Q_ASSERT(m_courses.contains(language->id()));
    Q_ASSERT(index >= 0 && index < m_courses[language->id()].count());

    return m_courses[language->id()].at(index);
}

void ContributorRepository::reloadCourseOrSkeleton(std::shared_ptr<ICourse> courseOrSkeleton)
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
        for (auto resource : m_skeletonResources) {
            if (resource->id() == courseOrSkeleton->id()) {
                // TODO no reload available
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

void ContributorRepository::updateCourseFromSkeleton(std::shared_ptr<IEditableCourse> course)
{
    //TODO implement status information that are shown at mainwindow
    if (course->foreignId().isEmpty())  {
        qCritical() << "No skeleton ID specified, aborting update.";
        return;
    }
    std::shared_ptr<ICourse> skeleton;
    for (const auto &iter : m_skeletonResources) {
        if (iter->id() == course->foreignId()) {
            skeleton = iter;
            break;
        }
    }
    if (!skeleton)  {
        qCritical() << "Could not find skeleton with id " << course->foreignId() << ", aborting update.";
        return;
    }

// FIXME memory handling logic is broken
//    // update now
//    for (Unit *unitSkeleton : skeleton->unitList()) {
//        // import unit if not exists
//        std::unique_ptr<Unit> currentUnit(new Unit);
//        bool found = false;
//        for (Unit *unit : course->unitList()) {
//            if (unit->foreignId() == unitSkeleton->id()) {
//                found = true;
//                currentUnit = unit;
//                break;
//            }
//        }
//        if (found == false) {
//            currentUnit = new Unit(course);
//            currentUnit->setId(QUuid::createUuid().toString());
//            currentUnit->setTitle(unitSkeleton->title());
//            currentUnit->setForeignId(unitSkeleton->id());
//            currentUnit->setCourse(course);
//            course->addUnit(std::move(currentUnit));
//        }

//        // update phrases
//        for (Phrase *phraseSkeleton : unitSkeleton->phraseList()) {
//            bool found = false;
//            for (Phrase *phrase : currentUnit->phraseList()) {
//                if (phrase->foreignId() == phraseSkeleton->id()) {
//                    if (phrase->i18nText() != phraseSkeleton->text()) {
//                        phrase->setEditState(Phrase::Unknown);
//                        phrase->seti18nText(phraseSkeleton->text());
//                    }
//                    found = true;
//                    break;
//                }
//            }
//            if (found == false) {
//                Phrase *newPhrase = new Phrase(course);
//                newPhrase->setForeignId(phraseSkeleton->id());
//                newPhrase->setId(QUuid::createUuid().toString());
//                newPhrase->setText(phraseSkeleton->text());
//                newPhrase->seti18nText(phraseSkeleton->text());
//                newPhrase->setType(phraseSkeleton->type());
//                newPhrase->setUnit(currentUnit.get());
//                currentUnit->addPhrase(newPhrase);
//            }
//        }
//    }
    // FIXME deassociate removed phrases
    qCDebug(ARTIKULATE_LOG) << "Update performed!";
}

std::shared_ptr<EditableCourseResource> ContributorRepository::addCourse(const QUrl &courseFile)
{
    std::shared_ptr<EditableCourseResource> course;

    // skip already loaded resources
    if (m_loadedResources.contains(courseFile.toLocalFile())) {
        // TODO return existing resource
    } else {
        course = EditableCourseResource::create(courseFile, this);
        if (course->language() == nullptr) {
            qCritical() << "Could not load course, language unknown:" << courseFile.toLocalFile();
            course.reset();
        } else { // this is the regular case
            m_loadedResources.append(courseFile.toLocalFile());

            const QString languageId = course->language()->id();
            Q_ASSERT(!languageId.isEmpty());
            if (!m_courses.contains(languageId)) {
                m_courses.insert(languageId, QVector<std::shared_ptr<EditableCourseResource>>());
            }
            emit courseAboutToBeAdded(course.get(), m_courses[course->language()->id()].count());
            m_courses[languageId].append(course);
            emit courseAdded();
            emit languageCoursesChanged();
        }
    }
    return course;
}

void ContributorRepository::removeCourse(std::shared_ptr<ICourse> course)
{
    for (int index = 0; index < m_courses[course->language()->id()].length(); ++index) {
        if (m_courses[course->language()->id()].at(index) == course) {
            emit courseAboutToBeRemoved(index);
            m_courses[course->language()->id()].removeAt(index);
            emit courseRemoved();
            return;
        }
    }
}

IEditableCourse * ContributorRepository::createCourse(std::shared_ptr<Language> language, std::shared_ptr<SkeletonResource> skeleton)
{
    // set path
    QString path = QStringLiteral("%1/%2/%3/%4/%4.xml")
        .arg(storageLocation(),
             QStringLiteral("courses"),
             skeleton->id(),
             language->id());

    auto course = EditableCourseResource::create(QUrl::fromLocalFile(path), this);

    Q_ASSERT(course);
    course->setId(QUuid::createUuid().toString());
    course->setTitle(skeleton->title());
    course->setDescription(skeleton->description());
    course->setLanguage(language);
    course->setForeignId(skeleton->id());

    return course.get();
}

std::shared_ptr<IEditableCourse> ContributorRepository::addSkeleton(const QUrl &file)
{
    std::shared_ptr<IEditableCourse> resource;

    // skip already loaded resources
    if (m_loadedResources.contains(file.toLocalFile())) {
        qCInfo(ARTIKULATE_LOG()) << "Skeleton already loaded, using known resource:" << file;
        for (auto skeleton : m_skeletonResources) {
            if (skeleton->file() == file) {
                resource = skeleton;
                break;
            }
        }
    } else {
        resource = SkeletonResource::create(file, this);
        m_loadedResources.append(resource->file().toLocalFile());
        emit skeletonAboutToBeAdded(resource.get(), m_skeletonResources.count());
        m_skeletonResources.append(resource);
        emit skeletonAdded();
    }
    return resource;
}

void ContributorRepository::removeSkeleton(SkeletonResource *skeleton)
{
    for (int index = 0; index < m_skeletonResources.length(); ++index) {
        if (m_skeletonResources.at(index)->id() == skeleton->id()) {
            emit skeletonAboutToBeRemoved(index, index);
            m_skeletonResources.removeAt(index);
            emit skeletonRemoved();
            return;
        }
    }
}

QVector<std::shared_ptr<IEditableCourse>> ContributorRepository::skeletons() const
{
    QVector<std::shared_ptr<IEditableCourse>> skeletonList;
    for (const auto &skeleton : m_skeletonResources) {
        skeletonList.append(skeleton);
    }
    return skeletonList;
}
