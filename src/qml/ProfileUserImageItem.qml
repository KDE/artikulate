/*
 *  Copyright 2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
    height: 120
    width: height

    property Learner profile: null

    Component {
        id: profileImage
        Image {
            Connections {
                target: profile
                onImageUrlChanged: {
                    // trigger reload
                    source = ""
                    source = profile.imageUrl
                }
            }
            anchors.fill: parent
            fillMode: Image.Pad
            cache: false
            source: profile.imageUrl
        }
    }

    Component {
        id: dummyImage
        Icon {
            anchors.fill: parent
            icon: "user-identity"
        }
    }

    Loader {
        anchors.fill: parent
        sourceComponent: profile.imageUrl === "" ? dummyImage : profileImage
    }
}
