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

#include "skeleton.h"
#include "resources/skeletonresource.h"
#include "course.h"
#include "unit.h"
#include "language.h"
#include "resourcemanager.h"
#include "phonemegroup.h"

#include "artikulate_debug.h"
#include <KLocalizedString>
#include <QStringList>
#include <QPair>
#include <QUuid>

Skeleton::Skeleton(ResourceInterface *resource)
    : Course(resource)
    , m_resource(qobject_cast<SkeletonResource*>(resource))
{
}

void Skeleton::sync()
{
    if (!file().isValid() || file().isEmpty() || m_resource == nullptr) {
        qCWarning(ARTIKULATE_LOG()) << "No file path set, aborting sync operation.";
        return;
    }
    m_resource->sync();
    setModified(false);
}
