/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef RESOURCEREPOSITORY_H
#define RESOURCEREPOSITORY_H

#include "artikulatecore_export.h"
#include "iresourcerepository.h"
#include <QObject>
#include <QMap>
#include <QHash>
#include <QStringList>
#include <QVector>

class QUrl;
class CourseResource;
class ICourse;
class Language;
class LanguageResource;

/**
 * @class ResourceRepository
 *
 * This class provides data handling of all downloaded trainingdata of a user. It dervies from the repository interface
 * to provide a generalized access to the data.
 */
class ARTIKULATECORE_EXPORT ResourceRepository : public IResourceRepository
{
    Q_OBJECT
    Q_INTERFACES(IResourceRepository)

public:
    explicit ResourceRepository();
    ~ResourceRepository() override;

    /**
     * @brief Construtor for ResourceRepository object with explicitly set course folder
     *
     * @param storageLocation relative or absolute path to courses/ folder (including that directory)
     */
    explicit ResourceRepository(const QUrl &storageLocation);

    /**
     * @return path to repository location
     */
    QString storageLocation() const override;

    /**
     * @return list of available courses
     */
    QVector<std::shared_ptr<ICourse>> courses() const override;

    /**
     * @return list of available courses
     */
    QVector<std::shared_ptr<ICourse>> courses(const QString &languageId) const override;

    /**
     * @return list of all available language specifications
     */
    QVector<std::shared_ptr<Language>> languages() const override;

    Language * language(const QString &id) const;

public Q_SLOTS:
    /**
     * \brief updates available resources
     */
    void reloadCourses() override;

private:
    bool loadCourse(const QString &resourceFile);
    bool loadLanguage(const QString &resourceFile);
    QVector<std::shared_ptr<ICourse>> m_courses;
    QHash<QString, std::shared_ptr<Language>> m_languages; ///>! (language-identifier, language resource)
    QStringList m_loadedCourses;
    const QString m_storageLocation;
};

#endif // RESOURCEREPOSITORY_H
