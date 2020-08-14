/*
 *  SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef EDITABLEREPOSITORYSTUB_H
#define EDITABLEREPOSITORYSTUB_H

#include "core/ieditablecourse.h"
#include "core/ieditablerepository.h"
#include <QObject>
#include <QVector>

class ILanguage;
class SkeletonResource;

/**
 * @brief The EditableRepositoryStub is simple sub class only for testing
 */
class EditableRepositoryStub : public IEditableRepository
{
    Q_OBJECT
public:
    EditableRepositoryStub(std::vector<std::shared_ptr<ILanguage>> languages, std::vector<std::shared_ptr<IEditableCourse>> skeletons, std::vector<std::shared_ptr<IEditableCourse>> courses)
    {
        for (auto &language : languages) {
            m_languages.append(std::move(language));
        }
        for (auto &skeleton : skeletons) {
            m_skeletons.append(std::move(skeleton));
        }
        for (auto &course : courses) {
            m_courses.append(course);
        }
    }
    ~EditableRepositoryStub() override;
    QUrl storageLocation() const override
    {
        return QUrl();
    }
    QVector<std::shared_ptr<IEditableCourse>> skeletons() const override
    {
        return m_skeletons;
    }
    QVector<std::shared_ptr<IEditableCourse>> editableCourses() const override
    {
        return m_courses;
    }
    QVector<std::shared_ptr<ICourse>> courses() const override
    {
        QVector<std::shared_ptr<ICourse>> courses;
        for (auto course : m_courses) {
            courses.push_back(course);
        }
        return courses;
    }
    QVector<std::shared_ptr<ICourse>> courses(const QString &languageId) const override
    {
        Q_UNUSED(languageId);
        return QVector<std::shared_ptr<ICourse>>();
    }
    std::shared_ptr<IEditableCourse> editableCourse(std::shared_ptr<ILanguage> language, int index) const override
    {
        Q_UNUSED(language);
        Q_UNUSED(index);
        return std::shared_ptr<IEditableCourse>();
    }
    void reloadCourses() override
    {
        // do nothing
    }
    QVector<std::shared_ptr<ILanguage>> languages() const override
    {
        return m_languages;
    }
    void appendCourse(std::shared_ptr<IEditableCourse> course)
    {
        emit courseAboutToBeAdded(course, m_courses.count());
        m_courses.append(course);
        emit courseAdded();
    }

    void removeCourse(std::shared_ptr<IEditableCourse> course)
    {
        auto index = m_courses.indexOf(course);
        Q_ASSERT(index >= 0);
        if (index >= 0) {
            emit courseAboutToBeRemoved(index);
            m_courses.remove(index);
            emit courseRemoved();
        }
    }
    void appendSkeleton(std::shared_ptr<IEditableCourse> skeleton)
    {
        emit skeletonAboutToBeAdded(skeleton, m_skeletons.count());
        m_skeletons.append(skeleton);
        emit skeletonAdded();
    }

    void removeSkeleton(std::shared_ptr<IEditableCourse> skeleton)
    {
        auto index = m_skeletons.indexOf(skeleton);
        Q_ASSERT(index >= 0);
        if (index >= 0) {
            emit skeletonAboutToBeRemoved(index);
            m_skeletons.remove(index);
            emit skeletonRemoved();
        }
    }
    void updateCourseFromSkeleton(std::shared_ptr<IEditableCourse> course) override
    {
        Q_UNUSED(course);
        // do nothing
    }

private:
    QVector<std::shared_ptr<ILanguage>> m_languages;
    QVector<std::shared_ptr<IEditableCourse>> m_skeletons;
    QVector<std::shared_ptr<IEditableCourse>> m_courses;
};

#endif
