/*
 *  Copyright 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef EDITABLEREPOSITORYSTUB_H
#define EDITABLEREPOSITORYSTUB_H

#include "core/ieditablerepository.h"
#include "core/ieditablecourse.h"
#include <QObject>
#include <QVector>

class Language;
class SkeletonResource;

/**
 * @brief The EditableRepositoryStub is simple sub class only for testing
 */
class EditableRepositoryStub : public IEditableRepository
{
    Q_OBJECT
public:
    EditableRepositoryStub(
            std::vector<std::shared_ptr<Language>> languages,
            std::vector<std::shared_ptr<IEditableCourse>> skeletons,
            std::vector<std::shared_ptr<IEditableCourse>> courses)
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
    QString storageLocation() const override
    {
        return QString();
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
    std::shared_ptr<IEditableCourse> editableCourse(std::shared_ptr<Language> language, int index) const override
    {
        Q_UNUSED(language);
        Q_UNUSED(index);
        return std::shared_ptr<IEditableCourse>();
    }
    void reloadCourses() override
    {
        // do nothing
    }
    QVector<std::shared_ptr<Language>> languages() const override
    {
        return m_languages;
    }
    void updateCourseFromSkeleton(std::shared_ptr<IEditableCourse> course) override
    {
        Q_UNUSED(course);
        // do nothing
    }

Q_SIGNALS:
    void courseAboutToBeAdded(ICourse*,int) override;
    void courseAdded() override;
    void courseAboutToBeRemoved(int) override;
    void courseRemoved() override;

private:
    QVector<std::shared_ptr<Language>> m_languages;
    QVector<std::shared_ptr<IEditableCourse>> m_skeletons;
    QVector<std::shared_ptr<IEditableCourse>> m_courses;
};

#endif
