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

#ifndef CONTRIBUTORREPOSITORY_H
#define CONTRIBUTORREPOSITORY_H

#include "artikulatecore_export.h"
#include "ieditablerepository.h"
#include <QObject>
#include <QMap>
#include <QHash>
#include <QVector>
#include <QStringList>

#include "liblearnerprofile/src/learninggoal.h"

class SkeletonResource;
class EditableCourseResource;
class LanguageResource;
class Language;
class ICourse;
class QUrl;

/**
 * @class ContributorRepository
 * This class handles the resources of a contributor.
 */
class ARTIKULATECORE_EXPORT ContributorRepository : public IEditableRepository
{
    Q_OBJECT
    Q_INTERFACES(IResourceRepository)
    Q_INTERFACES(IEditableRepository)

    Q_PROPERTY(QString repositoryUrl READ storageLocation NOTIFY repositoryChanged)

public:
    explicit ContributorRepository(QObject *parent = nullptr);

    /**
     * save all changes to course resources
     */
    void sync();

    /**
     * \return \c true if any course or skeleton is modified, otherwise \c false
     */
    bool modified() const;

    /**
     * \return path to working repository, if one is set
     */
    QString storageLocation() const override;

    /**
     * Set path to central storage location
     * \param path the path to the storage location directory
     */
    void setStorageLocation(const QString &path);

    /**
     * \return list of all available language specifications
     */
    Q_DECL_DEPRECATED QList<LanguageResource *> languageResources() const;

    QVector<Language *> languages() const override;

    /**
     * \return language by \p index
     */
    Q_INVOKABLE Language * language(int index) const;

    /**
     * \return language by \p learningGoal
     */
    Q_INVOKABLE Language * language(LearnerProfile::LearningGoal* learningGoal) const;

    QVector<ICourse *> courses() const override;
    QVector<IEditableCourse *> editableCourses() const override;
    QVector<ICourse *> courses(Language *language) const override;

    /**
     * \return list of all loaded courses for language \p language
     */
    QList<EditableCourseResource *> courseResources(Language *language);

    Q_INVOKABLE IEditableCourse * editableCourse(Language *language, int index) const override;

    /**
     * Reset the file for this course or skeleton.
     *
     * \param course the course to be reloaded
     */
    Q_INVOKABLE void reloadCourseOrSkeleton(ICourse *course);

    /**
     * @brief Implementation of course resource reloading
     */
    void reloadCourses() override;

    /**
     * Imports units and phrases from skeleton, deassociates removed ones.
     *
     * \param course the course to be update
     */
    void updateCourseFromSkeleton(EditableCourseResource *course);

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
    EditableCourseResource * addCourse(const QUrl &courseFile);

    /**
     * Adds course to resource manager. If the course's language is not registered, the language
     * is registered by this method.
     *
     * \param resource the course resource to add to resource manager
     */
    void addCourseResource(EditableCourseResource *resource);

    /**
     * Remove course from resource manager. If the course is modified its changes are NOT
     * written. For writing changes, the Course::sync() method must be called directly.
     *
     * \param course is the course to be removed
     */
    void removeCourse(ICourse *course);

    /**
     * Create new course for \p language and derived from \p skeleton.
     *
     * \return created course
     */
    Q_INVOKABLE EditableCourseResource * createCourse(Language *language, SkeletonResource *skeleton);

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
    void removeSkeleton(SkeletonResource *skeleton);

    QVector<IEditableCourse *> skeletons() const override;

Q_SIGNALS:
    void languageResourceAdded();
    void languageResourceAboutToBeAdded(LanguageResource*,int);
    void languageResourceRemoved();
    void languageResourceAboutToBeRemoved(int);
    void repositoryChanged();
    void courseAdded() override;
    void courseAboutToBeAdded(ICourse*,int) override;
    void courseAboutToBeRemoved(int) override;
    void courseRemoved() override;
    void skeletonAdded();
    void skeletonAboutToBeAdded(ICourse*,int);
    void skeletonRemoved();
    void skeletonAboutToBeRemoved(int,int);
    void languageCoursesChanged();

private:
    /**
     * This method loads all language files that are provided in the standard directories
     * for this application.
     */
    void loadLanguageResources();
    QString m_storageLocation;
    QList<LanguageResource *> m_languageResources;
    QMap<QString, QList<EditableCourseResource *> > m_courses; //!> (language-id, course-resource)
    QVector<SkeletonResource *> m_skeletonResources;
    QStringList m_loadedResources;
};

#endif
