/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef SKELETON_H
#define SKELETON_H

#include "artikulatecore_export.h"
#include "course.h"
#include <QObject>
#include <QMap>
#include <QUrl>

class ResourceInterface;
class SkeletonResource;

class ARTIKULATECORE_EXPORT Skeleton : public Course
{
    Q_OBJECT

public:
    explicit Skeleton(ResourceInterface *resource = nullptr);
    ~Skeleton();

    /**
     * Writes course object back to file and set \ref modified state to false.
     * If no file is set, no operation is performed.
     */
    Q_INVOKABLE void sync() override;

private:
    Q_DISABLE_COPY(Skeleton)

    SkeletonResource * const m_resource;
};

#endif // SKELETON_H
