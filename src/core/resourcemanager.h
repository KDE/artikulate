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

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "artikulatecore_export.h"
#include <QObject>
#include <QMap>
#include <QHash>
#include <QStringList>

#include "liblearnerprofile/src/learninggoal.h"

class SkeletonResource;
class CourseResource;
class LanguageResource;
class Skeleton;
class Language;
class Course;
class ProfileManager;
class QUrl;

namespace LearnerProfile {
    class ProfileManager;
}

/**
 * \class ResourceManager
 * This class loads and stores all data files of the application.
 */
class ARTIKULATECORE_EXPORT ResourceManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isRepositoryManager READ isRepositoryManager NOTIFY repositoryChanged);
    Q_PROPERTY(QString repositoryUrl READ repositoryUrl NOTIFY repositoryChanged);

public:
    explicit ResourceManager(QObject *parent = nullptr);

    /**
     * Load all course resources.
     * This loading is very fast, since course files are only partly (~20 top lines) parsed and
     * the complete parsing is postproned until first access.
     */
    void loadCourseResources();

    /**
     * This method loads all language files that are provided in the standard directories
     * for this application.
     */
    void loadLanguageResources();

    /**
     * save all changes to course resources
     */
    void sync();

    /**
     * \return \c true if any course or skeleton is modified, otherwise \c false
     */
    bool modified() const;

    /**
     * Register loaded languages as learning goals in profile manager.
     *
     * TODO this should not be done by explicit call, but by putting data at shared data storage
     *      for profile manager
     */
    void registerLearningGoals(LearnerProfile::ProfileManager *profileManger);

    /**
     * \return \c true if a repository is used, else \c false
     */
    Q_INVOKABLE bool isRepositoryManager() const;

    /**
     * \return path to working repository, if one is set
     */
    QString repositoryUrl() const;

    /**
     * \return list of all available language specifications
     */
    QList<LanguageResource *> languageResources() const;

    /**
     * \return language by \p index
     */
    Q_INVOKABLE Language * language(int index) const;

    /**
     * \return language by \p learningGoal
     */
    Q_INVOKABLE Language * language(LearnerProfile::LearningGoal* learningGoal) const;

    /**
     * \return list of all loaded courses for language \p language
     */
    QList<CourseResource *> courseResources(Language *language);

    Q_INVOKABLE Course * course(Language *language, int index) const;

    /**
     * Reset the file for this course or skeleton.
     *
     * \param course the course to be reloaded
     */
    Q_INVOKABLE void reloadCourseOrSkeleton(Course *course);

    /**
     * Imports units and phrases from skeleton, deassociates removed ones.
     *
     * \param course the course to be update
     */
    void updateCourseFromSkeleton(Course *course);

    /**
     * Add language to resource manager by parsing the given language specification file.
     *
     * \param languageFile is the local XML file containing the language
     */
    void addLanguage(const QUrl &languageFile);

    /**
     * Adds course to resource manager by parsing the given course specification file.
     *
     * \param courseFile is the local XML file containing the course
     * \return true if loaded successfully, otherwise false
     */
    CourseResource * addCourse(const QUrl &courseFile);

    /**
     * Adds course to resource manager. If the course's language is not registered, the language
     * is registered by this method.
     *
     * \param resource the course resource to add to resource manager
     */
    void addCourseResource(CourseResource *resource);

    /**
     * Remove course from resource manager. If the course is modified its changes are NOT
     * written. For writing changes, the Course::sync() method must be called directly.
     *
     * \param course is the course to be removed
     */
    void removeCourse(Course *course);

    /**
     * Create new course for \p language and derived from \p skeleton.
     *
     * \return created course
     */
    Q_INVOKABLE Course * createCourse(Language *language, Skeleton *skeleton);

    /**
     * Adds skeleton resource to resource manager
     *
     * \param resource the skeleton resource to add to resource manager
     */
    void addSkeleton(const QUrl &skeletonFile);

    /**
     * Adds skeleton resource to resource manager
     *
     * \param resource the skeleton resource to add to resource manager
     */
    void addSkeletonResource(SkeletonResource *resource);

    /**
     * Remove skeleton from resource manager. If the skeleton is modified its changes are NOT
     * written. For writing changes, the Skeleton::sync() method must be called directly.
     *
     * \param skeleton is the skeleton to be removed
     */
    void removeSkeleton(Skeleton *skeleton);

    /**
     * \return list of all loaded skeletons resources
     */
    QList<SkeletonResource *> skeletonResources();

Q_SIGNALS:
    void languageResourceAdded();
    void languageResourceAboutToBeAdded(LanguageResource*,int);
    void languageResourceRemoved();
    void languageResourceAboutToBeRemoved(int);
    void repositoryChanged();
    void courseResourceAdded();
    void courseResourceAboutToBeAdded(CourseResource*,int);
    void courseResourceAboutToBeRemoved(int);
    void skeletonAdded();
    void skeletonAboutToBeAdded(Course*,int);
    void skeletonRemoved();
    void skeletonAboutToBeRemoved(int,int);
    void languageCoursesChanged();

private:
    QList<LanguageResource *> m_languageResources;
    QMap<QString, QList<CourseResource *> > m_courseResources; //!> (language-id, course-resource)
    QList<SkeletonResource *> m_skeletonResources;
    QStringList m_loadedResources;
};

#endif // RESOURCEMANAGER_H
