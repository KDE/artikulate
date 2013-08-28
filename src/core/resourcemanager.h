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


#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "artikulatecore_export.h"
#include <QObject>
#include <QMap>
#include <QHash>
#include <QStringList>

class SkeletonResource;
class CourseResource;
class LanguageResource;
class Skeleton;
class Language;
class Course;
class KUrl;
class QDomDocument;
class QFile;
class QXmlSchema;

/**
 * \class ResourceManager
 * This class loads and stores all data files of the application.
 */
class ARTIKULATELIB_EXPORT ResourceManager : public QObject
{
    Q_OBJECT
public:
    explicit ResourceManager(QObject *parent = 0);

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
     * returns true if a repository is used, else false
     */
    Q_INVOKABLE bool isRepositoryManager() const;

    /**
     * \return list of all available language specifications
     */
    QList<LanguageResource *> languageResources() const;

    Q_INVOKABLE Language * language(int index) const;

    /**
     * \return list of all loaded courses for language \p language
     */
    QList<CourseResource *> courseResources(Language *language);

    Q_INVOKABLE Course * course(Language *language, int index) const;

    /**
     * Load course from locally stored XML file.
     * TODO allow loading of remote XML files
     *
     * \param courseFile is the local XML file containing the course
     * \return loaded course
     */
    Course * loadCourse(const KUrl &courseFile);

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
    Q_INVOKABLE void updateCourseFromSkeleton(Course *course);

    /**
     * Adds course to resource manager by parsing the given course specification file.
     *
     * \param courseFile is the local XML file containing the course
     * \return true if loaded successfully, otherwise false
     */
    bool addCourse(const KUrl &courseFile);

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
     * Adds skeleton resource to resource manager
     *
     * \param resource the skeleton resource to add to resource manager
     */
    void addSkeleton(const KUrl &skeletonFile);

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

    Q_INVOKABLE void newCourseDialog();

    /**
     * Serializes course object to its file. The course must have a valid local file path set.
     *
     * \param course is the course to be serialized
     */
    static void syncCourse(Course *course);

signals:
    void languageAdded();
    void languageAboutToBeAdded(Language*,int);
    void languageRemoved();
    void languageAboutToBeRemoved(int,int);
    void courseAdded();
    void courseAboutToBeAdded(Course*,int);
    void courseRemoved();
    void courseAboutToBeRemoved(int,int);
    void skeletonAdded();
    void skeletonAboutToBeAdded(Course*,int);
    void skeletonRemoved();
    void skeletonAboutToBeRemoved(int,int);

private:
    QList<LanguageResource *> m_languageResources;
    QMap<QString, QList<CourseResource *> > m_courseResources; //!> (language-id, course-resource)
    QList<SkeletonResource *> m_skeletonList;
    QStringList m_loadedResources;
};

#endif // RESOURCEMANAGER_H
