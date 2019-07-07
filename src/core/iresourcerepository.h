/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef IRESOURCEREPOSITORY_H
#define IRESOURCEREPOSITORY_H

#include "artikulatecore_export.h"
#include <memory>
#include <QObject>

class ICourse;
class Language;

/**
 * \class IResourceRepository
 * This interface provides a generic interface that provides just the methods and signals needed
 * to integrade a repository into the training part of Artikulate.
 */
class ARTIKULATECORE_EXPORT IResourceRepository : public QObject
{
    Q_OBJECT
public:
    virtual ~IResourceRepository() = default;

    /**
     * \return path to working repository, if one is set
     */
    virtual QString storageLocation() const = 0;

    /**
     * @return list of all loaded courses
     */
    virtual QVector<std::shared_ptr<ICourse>> courses() const = 0;

    /**
     * @param language to use for filtering
     * @return list of all loaded courses filtered by the named language
     */
    virtual QVector<std::shared_ptr<ICourse>> courses(const QString &languageId) const = 0;

    /**
     * @brief Requests a refresh of all resources
     *
     * Typical reasons to call this are GHNS signals
     */
    virtual void reloadCourses() = 0;

    /**
     * \return list of all available languages
     */
    virtual QVector<std::shared_ptr<Language>> languages() const = 0;

Q_SIGNALS:
    void courseAboutToBeAdded(std::shared_ptr<ICourse>,int);
    void courseAdded();
    void courseAboutToBeRemoved(int);
    void courseRemoved();
};
Q_DECLARE_INTERFACE(IResourceRepository, "IResourceRepository")

#endif
