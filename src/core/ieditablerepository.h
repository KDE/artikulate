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

#ifndef IEDITABLEREPOSITORY_H
#define IEDITABLEREPOSITORY_H

#include "artikulatecore_export.h"
#include "iresourcerepository.h"

class IEditableCourse;
class Language;

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
    virtual QVector<IEditableCourse *> editableCourses() const = 0;
    virtual IEditableCourse * editableCourse(Language *language, int index) const = 0;
    virtual QVector<IEditableCourse *> skeletons() const = 0;
};
Q_DECLARE_INTERFACE(IEditableRepository, "IEditableRepository")

#endif
