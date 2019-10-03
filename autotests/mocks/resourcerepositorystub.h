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

#ifndef RESOURCEREPOSITORYSTUB_H
#define RESOURCEREPOSITORYSTUB_H

#include "core/iresourcerepository.h"
#include "core/language.h"
#include <QObject>
#include <QVector>
#include <QDebug>

class ICourse;

/**
 * @brief The ResourceRepositoryStub that only provides languages and a storage location, but nothing else
 */
class ResourceRepositoryStub : public IResourceRepository
{
    Q_OBJECT
public:
    ResourceRepositoryStub(std::vector<std::unique_ptr<ILanguage>> languages)
    {
        for (auto &language : languages) {
            m_languages.append(std::move(language));
        }
    }

    ResourceRepositoryStub(std::vector<std::shared_ptr<ILanguage>> languages)
    {
        for (auto &language : languages) {
            m_languages.append(language);
        }
    }

    ResourceRepositoryStub(std::vector<std::shared_ptr<ILanguage>> languages, std::vector<std::shared_ptr<ICourse>> courses)
    {
        for (auto &language : languages) {
            m_languages.append(language);
        }
        for (auto &course : courses) {
            m_courses.append(course);
        }
    }

    ~ResourceRepositoryStub() override;

    QUrl storageLocation() const override
    {
        return m_storageLocation;
    }

    QVector<std::shared_ptr<ICourse>> courses() const override
    {
        return m_courses;
    }

    QVector<std::shared_ptr<ICourse>> courses(const QString &languageId) const override
    {
        Q_UNUSED(languageId);
        return m_courses; // do not filter by languages
    }

    void reloadCourses() override
    {
        ; // do nothing, stub shall only provide languages
    }

    QVector<std::shared_ptr<ILanguage>> languages() const override
    {
        return m_languages;
    }

    void appendCourse(std::shared_ptr<ICourse> course)
    {
        emit courseAboutToBeAdded(course, m_courses.count());
        m_courses.append(course);
        emit courseAdded();
    }

    void removeCourse(std::shared_ptr<ICourse> course)
    {
        auto index = m_courses.indexOf(course);
        Q_ASSERT(index >= 0);
        if (index >= 0) {
            emit courseAboutToBeRemoved(index);
            m_courses.remove(index);
            emit courseRemoved();
        }
    }

private:
    QUrl m_storageLocation;
    QVector<std::shared_ptr<ILanguage>> m_languages;
    QVector<std::shared_ptr<ICourse>> m_courses;
};

#endif
