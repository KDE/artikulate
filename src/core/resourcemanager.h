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
     * Generate or update the cache of all resource files. This method must be called after
     * switching from locally installed courses to repository.
     */
    void updateResourceFileCache();

    /**
     * returns true if a repository is used, else false
     */
    Q_INVOKABLE bool isRepositoryManager() const;

    /**
     * This method loads all language and course files that are provided in the standard directories
     * for this application.
     */
    void loadResources();

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

    /**
     * Serializes skeleton to its file. The skeleton must have a valid local file path set.
     *
     * \param skeleton is the course skeleton to be serialized
     */
    static void syncSkeleton(Skeleton *skeleton);

    /**
     * \deprecated
     * Load XSD file given by its file name (without ".xsd" suffix). The method searches exclusively
     * the standard install dir for XSD files in subdirectory "schemes/".
     *
     * \param schemeName name of the Xml schema without suffix
     * \return loaded XML Schema
     */
    QXmlSchema loadXmlSchema(const QString &schemeName) const;

    /**
     * \deprecated
     * Load XML file given by \p file that confirms with XML schema \p scheme.
     *
     * \param path is the path to the XML file to be loaded
     * \param scheme is the XML schema describing the DOM
     * \return the loaded DOM document
     */
    QDomDocument loadDomDocument(const KUrl &path, const QXmlSchema &schema) const;

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
    QMultiMap<QString, KUrl> m_courseFileCache; //!> language identifier, path
    QList<KUrl> m_skeletonFileCache;
    QList<LanguageResource *> m_languageResources;
    QMap<QString, QList<CourseResource *> > m_courseResources; //!> (language-id, course-resource)
    QList<SkeletonResource *> m_skeletonList;
    bool m_loadingSkeletons;
};

#endif // RESOURCEMANAGER_H
