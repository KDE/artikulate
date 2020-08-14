/*
 *  SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef IEDITABLEREPOSITORY_H
#define IEDITABLEREPOSITORY_H

#include "artikulatecore_export.h"
#include "iresourcerepository.h"
#include <memory>

class IEditableCourse;
class ILanguage;

/**
 * \class IEditableRepository
 * This interface provides a generic interface that provides just the methods and signals needed
 * to integrade a repository into the editing part of Artikulate.
 */
class ARTIKULATECORE_EXPORT IEditableRepository : public IResourceRepository
{
    Q_OBJECT
public:
    virtual ~IEditableRepository() = default;
    virtual QVector<std::shared_ptr<IEditableCourse>> editableCourses() const = 0;
    virtual std::shared_ptr<IEditableCourse> editableCourse(std::shared_ptr<ILanguage> language, int index) const = 0;
    virtual QVector<std::shared_ptr<IEditableCourse>> skeletons() const = 0;
    /**
     * Imports units and phrases from skeleton, deassociates removed ones.
     *
     * \param course the course to be updated
     */
    virtual void updateCourseFromSkeleton(std::shared_ptr<IEditableCourse> course) = 0;

Q_SIGNALS:
    void skeletonAboutToBeAdded(std::shared_ptr<IEditableCourse>, int);
    void skeletonAdded();
    void skeletonAboutToBeRemoved(int);
    void skeletonRemoved();
};
Q_DECLARE_INTERFACE(IEditableRepository, "IEditableRepository")

#endif
