/*
 *  Copyright 2012       Sebastian Gottfried <sebastiangottfried@web.de>
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

    property alias title: label.text
    property string iconName
    property alias label: label
    property bool checked: false

    signal stateChanged(bool checked)

    height: 15 + 30 + 15
    state: mouseArea.containsMouse? "hover": "normal"

    Rectangle {
        id: bg
        radius: 8
        anchors.fill: parent
    }

    Item {
        id: padding
        opacity: root.checked ? 1.0 : 0.5

        anchors {
            fill: parent
            topMargin: 5
            rightMargin: 5
            bottomMargin: 5
            leftMargin: 5
        }

        width: parent.width
        height: Math.max(label.height, label.height)

        Icon {
            id: iconItem
            visible: !!root.iconName
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            icon: root.iconName
            width: theme.smallIconSize
            height: theme.smallIconSize
            enabled: root.checked
        }

        Label {
            id: label
            elide: Text.ElideRight
            anchors {
                left: iconItem.visible? iconItem.right: parent.left
                leftMargin: iconItem.visible ? Math.floor(theme.smallIconSize/2) : 0
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            height: paintedHeight
            font.italic: !root.checked
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        onClicked: {
            root.checked = !root.checked
            root.stateChanged(root.checked)
        }
    }

    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: bg
                opacity: 0
            }
        },
        State {
            name: "hover"
            PropertyChanges {
                target: bg
                opacity: 1.7
            }
        }
    ]
}
