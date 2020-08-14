/*
 *  SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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
