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

class ICourse;

/**
 * @brief The ResourceRepositoryStub that only provides languages and a storage location, but nothing else
 */
class ResourceRepositoryStub : public IResourceRepository
{
    Q_OBJECT
public:
    ResourceRepositoryStub(std::vector<std::unique_ptr<Language>> languages)
    {
        for (auto &language : languages) {
            m_languages.append(std::move(language));
        }
    }

    ResourceRepositoryStub(std::vector<std::shared_ptr<Language>> languages)
    {
        for (auto &language : languages) {
            m_languages.append(language);
        }
    }

    ~ResourceRepositoryStub() override;

    QString storageLocation() const override
    {
        return m_storageLocation;
    }

    QVector<std::shared_ptr<ICourse>> courses() const override
    {
        return QVector<std::shared_ptr<ICourse>>(); // do not return any courses: stub shall only provide languages
    }

    QVector<std::shared_ptr<ICourse>> courses(const QString &languageId) const override
    {
        Q_UNUSED(languageId);
        return QVector<std::shared_ptr<ICourse>>(); // do not return any courses: stub shall only provide languages
    }

    void reloadCourses() override
    {
        ; // do nothing, stub shall only provide languages
    }

    QVector<std::shared_ptr<Language>> languages() const override
    {
        return m_languages;
    }
Q_SIGNALS:
    void courseAboutToBeAdded(ICourse*,int) override;
    void courseAdded() override;
    void courseAboutToBeRemoved(int) override;
    void courseRemoved() override;

private:
    QString m_storageLocation;
    QVector<std::shared_ptr<Language>> m_languages;
};

#endif
