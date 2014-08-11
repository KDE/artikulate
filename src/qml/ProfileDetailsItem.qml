/*
 *  Copyright 2012  Sebastian Gottfried <sebastiangottfried@web.de>
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

import QtQuick 2.1
import QtQuick.Controls 1.2
import artikulate 1.0

Item {
    id: root

    property Learner profile: null
    property ProfileManager manager: profileManager

    signal deletionRequest();
/*
    Locale.Locale {
        id: locale
    }*/

//FIXME
//     Item {
//         id: infoContainer
//         width: root.width
//         height: root.height
//         anchors.centerIn: parent
//         visible: profile != null
//
//         PlasmaComponents.TabBar {
//             id: tabbar
//             width: parent.width
//             anchors { top: parent.top }
//
//             PlasmaComponents.TabButton {
//                 text: i18n("Learner")
//                 tab: userPage
//             }
//             PlasmaComponents.TabButton {
//                 text: i18n("Favorite Languages")
//                 tab: favoriteLanguages
//             }
//         }
//
//         PlasmaComponents.TabGroup {
//             width: parent.width
//             height: root.height - tabbar.height
//             anchors {
//                 top: tabbar.bottom
//                 left: tabbar.left
//                 right: tabbar.right
//             }
//             ProfileDetailsViewUser {
//                 id: userPage
//                 profile: root.profile
//                 onDeletionRequest: {
//                     root.deletionRequest()
//                 }
//             }
//
//             ProfileDetailsViewFavorites {
//                 id: favoriteLanguages
//                 anchors.fill: parent
//                 width: parent.width
//                 height: parent.height
//                 profile: root.profile
//                 profileManager: root.manager
//             }
//         }
//     }
}
