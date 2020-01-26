/*
 *  Copyright 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef RESOURCEREPOSITORYSTUB_H
#define RESOURCEREPOSITORYSTUB_H

#include "core/iresourcerepository.h"
#include "core/language.h"
#include <QDebug>
#include <QObject>
#include <QVector>

class ICourse;

/**
 * @brief The ResourceRepositoryStub that only provides languages and a storage location, but nothing else
 */
class ResourceRepositoryStub : public IResourceRepository
{
    Q_OBJECT
public:
    explicit ResourceRepositoryStub(std::vector<std::unique_ptr<ILanguage>> languages)
    {
        for (auto &language : languages) {
            m_languages.append(std::move(language));
        }
    }

    explicit ResourceRepositoryStub(std::vector<std::shared_ptr<ILanguage>> languages)
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
