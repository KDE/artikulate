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

#ifndef VERSION_H
#define VERSION_H

#include <kdeversion.h>

#define ARTIKULATE_MAJOR_VERSION @ARTIKULATE_VERSION_MAJOR@
#define ARTIKULATE_MINOR_VERSION @ARTIKULATE_VERSION_MINOR@
#define ARTIKULATE_PATCH_VERSION @ARTIKULATE_VERSION_PATCH@

#define ARTIKULATE_VERSION_STR "@ARTIKULATE_VERSION_MAJOR@.@ARTIKULATE_VERSION_MINOR@.@ARTIKULATE_VERSION_PATCH@"

#define ARTIKULATE_VERSION KDE_MAKE_VERSION(@ARTIKULATE_VERSION_MAJOR@, @ARTIKULATE_VERSION_MINOR@, @ARTIKULATE_VERSION_PATCH@)

#endif

// define with 0/1 value
#cmakedefine01 USE_QTMULTIMEDIA
