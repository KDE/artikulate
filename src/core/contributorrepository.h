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
#include <QHash>
#include <QMap>
#include <QObject>
#include <QStringList>
#include <QUrl>
#include <QVector>
#include <memory>

#include "liblearnerprofile/src/learninggoal.h"

class SkeletonResource;
class EditableCourseResource;
class LanguageResource;
class Language;
class ICourse;

/**
 * @class ContributorRepository
 * This class handles the resources of a contributor.
 */
class ARTIKULATECORE_EXPORT ContributorRepository : public IEditableRepository
{
    Q_OBJECT
    Q_INTERFACES(IResourceRepository)
    Q_INTERFACES(IEditableRepository)

    Q_PROPERTY(QUrl repositoryUrl READ storageLocation WRITE setStorageLocation NOTIFY repositoryChanged)

public:
    explicit ContributorRepository();
    explicit ContributorRepository(QUrl storageLocation);
    ~ContributorRepository() override;

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
    QUrl storageLocation() const override;

    /**
     * Set path to central storage location
     * \param path the path to the storage location directory
     */
    void setStorageLocation(const QUrl &path);

    QVector<std::shared_ptr<ILanguage>> languages() const override;

    /**
     * \return language by \p index
     */
    std::shared_ptr<ILanguage> language(int index) const;

    /**
     * \return language by \p learningGoal
     */
    Q_INVOKABLE ILanguage *language(LearnerProfile::LearningGoal *learningGoal) const;

    QVector<std::shared_ptr<ICourse>> courses() const override;
    QVector<std::shared_ptr<ICourse>> courses(const QString &languageId) const override;
    QVector<std::shared_ptr<IEditableCourse>> editableCourses() const override;

    /**
     * \return list of all loaded courses for language \p language
     */
    QVector<std::shared_ptr<EditableCourseResource>> courseResources(std::shared_ptr<ILanguage> language);

    std::shared_ptr<IEditableCourse> editableCourse(std::shared_ptr<ILanguage> language, int index) const override;

    /**
     * Reset the file for this course or skeleton.
     *
     * \param course the course to be reloaded
     */
    void reloadCourseOrSkeleton(std::shared_ptr<ICourse> course);

    /**
     * @brief Implementation of course resource reloading
     */
    void reloadCourses() override;

    void updateCourseFromSkeleton(std::shared_ptr<IEditableCourse> course) override;

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
    std::shared_ptr<EditableCourseResource> addCourse(const QUrl &courseFile);

    /**
     * Adds course to resource manager. If the course's language is not registered, the language
     * is registered by this method.
     *
     * \param resource the course resource to add to resource manager
     */
    std::shared_ptr<EditableCourseResource> addCourseResource(std::unique_ptr<EditableCourseResource> resource);

    /**
     * Remove course from resource manager. If the course is modified its changes are NOT
     * written. For writing changes, the Course::sync() method must be called directly.
     *
     * \param course is the course to be removed
     */
    void removeCourse(std::shared_ptr<ICourse> course);

    /**
     * Create new course for \p language and derived from \p skeleton.
     *
     * \return created course
     */
    Q_INVOKABLE IEditableCourse *createCourse(std::shared_ptr<ILanguage> language, std::shared_ptr<SkeletonResource> skeleton);

    /**
     * Adds skeleton resource to resource manager
     *
     * \param resource the skeleton resource to add to resource manager
     */
    std::shared_ptr<IEditableCourse> addSkeleton(const QUrl &skeletonFile);

    /**
     * Remove skeleton from resource manager. If the skeleton is modified its changes are NOT
     * written. For writing changes, the Skeleton::sync() method must be called directly.
     *
     * \param skeleton is the skeleton to be removed
     */
    void removeSkeleton(SkeletonResource *skeleton);

    QVector<std::shared_ptr<IEditableCourse>> skeletons() const override;

Q_SIGNALS:
    void languageResourceAdded();
    void languageResourceAboutToBeAdded(std::shared_ptr<ILanguage>, int);
    void languageResourceRemoved();
    void languageResourceAboutToBeRemoved(int);
    void repositoryChanged();
    void skeletonAdded();
    void skeletonAboutToBeAdded(ICourse *, int);
    void skeletonRemoved();
    void skeletonAboutToBeRemoved(int, int);
    void languageCoursesChanged();

private:
    /**
     * This method loads all language files that are provided in the standard directories
     * for this application.
     */
    void loadLanguageResources();
    QUrl m_storageLocation;
    QVector<std::shared_ptr<ILanguage>> m_languages;
    QMap<QString, QVector<std::shared_ptr<EditableCourseResource>>> m_courses; //!> (language-id, course-resource)
    QVector<std::shared_ptr<IEditableCourse>> m_skeletonResources;
    QStringList m_loadedResources;
};

#endif
