/*
 *  SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "contributorrepository.h"
#include "artikulate_debug.h"
#include "language.h"
#include "liblearnerprofile/src/learninggoal.h"
#include "liblearnerprofile/src/profilemanager.h"
#include "phoneme.h"
#include "phonemegroup.h"
#include "phrase.h"
#include "resources/editablecourseresource.h"
#include "resources/skeletonresource.h"
#include "unit.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QStandardPaths>
#include <QUrl>
#include <QUuid>

ContributorRepository::ContributorRepository()
    : IEditableRepository()
{
    loadLanguageResources();
}

ContributorRepository::ContributorRepository(QUrl storageLocation)
    : IEditableRepository()
    , m_storageLocation(std::move(storageLocation))
{
    loadLanguageResources();
}

ContributorRepository::~ContributorRepository() = default;

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
    for (auto iter = m_courses.begin(); iter != m_courses.end(); ++iter) {
        for (auto course : iter.value()) {
            course->sync();
        }
    }
    for (auto skeleton : m_skeletonResources) {
        skeleton->sync();
    }
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

    auto language = Language::create(languageFile);

    emit languageResourceAboutToBeAdded(language, m_languages.count());
    m_languages.append(language);
    m_loadedResources.append(languageFile.toLocalFile());
    m_courses.insert(language->id(), QVector<std::shared_ptr<EditableCourseResource>>());
    emit languageResourceAdded();
}

QUrl ContributorRepository::storageLocation() const
{
    return m_storageLocation;
}

void ContributorRepository::setStorageLocation(const QUrl &path)
{
    m_storageLocation = path;
    emit repositoryChanged();
    reloadCourses();
}

QVector<std::shared_ptr<ILanguage>> ContributorRepository::languages() const
{
    return m_languages;
}

std::shared_ptr<ILanguage> ContributorRepository::language(int index) const
{
    Q_ASSERT(index >= 0 && index < m_languages.count());
    return m_languages.at(index);
}

ILanguage *ContributorRepository::language(LearnerProfile::LearningGoal *learningGoal) const
{
    if (!learningGoal) {
        return nullptr;
    }
    if (learningGoal->category() != LearnerProfile::LearningGoal::Language) {
        qCritical() << "Cannot translate non-language learning goal to language";
        return nullptr;
    }
    for (auto language : m_languages) {
        if (language->id() == learningGoal->identifier()) {
            return language.get();
        }
    }
    qCritical() << "No language registered with identifier " << learningGoal->identifier() << ": aborting";
    return nullptr;
}

QVector<std::shared_ptr<EditableCourseResource>> ContributorRepository::courseResources(std::shared_ptr<ILanguage> language)
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

std::shared_ptr<IEditableCourse> ContributorRepository::editableCourse(std::shared_ptr<ILanguage> language, int index) const
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
        // TODO better add a check if this is contained in the course list
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
    QDir skeletonDirectory = QDir(storageLocation().toLocalFile());

    skeletonDirectory.setFilter(QDir::Files | QDir::Hidden);
    if (!skeletonDirectory.cd(QStringLiteral("skeletons"))) {
        qCritical() << "There is no subdirectory \"skeletons\" in directory " << skeletonDirectory.path() << " cannot load skeletons.";
    } else {
        // read skeletons
        QFileInfoList list = skeletonDirectory.entryInfoList();
        for (int i = 0; i < list.size(); ++i) {
            QFileInfo fileInfo = list.at(i);
            addSkeleton(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
        }
    }

    // register contributor course files
    QDir courseDirectory(storageLocation().toLocalFile());
    if (!courseDirectory.cd(QStringLiteral("courses"))) {
        qCritical() << "There is no subdirectory \"courses\" in directory " << courseDirectory.path() << " cannot load courses.";
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
    // TODO this signal should only be emitted when repository was added/removed
    // yet the call to this method is very seldom and emitting it too often is not that harmful
    emit repositoryChanged();
}

void ContributorRepository::updateCourseFromSkeleton(std::shared_ptr<IEditableCourse> course)
{
    // TODO implement status information that are shown at mainwindow
    if (course->foreignId().isEmpty()) {
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
    if (!skeleton) {
        qCritical() << "Could not find skeleton with id " << course->foreignId() << ", aborting update.";
    } else {
        course->updateFrom(skeleton);
    }
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
            emit courseAboutToBeAdded(course, m_courses[course->language()->id()].count());
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

IEditableCourse *ContributorRepository::createCourse(std::shared_ptr<ILanguage> language, std::shared_ptr<SkeletonResource> skeleton)
{
    // set path
    QString path = QStringLiteral("%1/%2/%3/%4/%4.xml").arg(storageLocation().toLocalFile(), QStringLiteral("courses"), skeleton->id(), language->id());

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
