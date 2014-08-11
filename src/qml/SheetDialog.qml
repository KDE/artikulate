/*
 *  Copyright 2012  Sebastian Gottfried <sebastiangottfried@web.de>
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

Item {
    id: root

    property alias content: contentArea.children
    property int innerMargin: 20
    signal opended
    signal closed

    function open() {
        root.state = "open"
        opended()
    }

    function close() {
        root.state = "closed"
        closed()
    }

    function isOpen() {
        return root.state == "open"
    }

    clip: true
    visible: bg.opacity > 0

    /* swallow all mouse events */
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
    }

    Rectangle {
        id: bg
        anchors.fill: parent
        color: theme.backgroundColor
        opacity: 1.0

        Behavior on opacity {
            NumberAnimation {
                duration: 300
            }
        }
    }

    Item {
        id: contentArea
        anchors {
            fill: parent
            topMargin: 10
            rightMargin: 10
            bottomMargin: 10
            leftMargin: 10
        }
    }

    state: "closed"

    states: [
        State {
            name: "open"
            PropertyChanges { target: bg; opacity: 1.0 }
            PropertyChanges { target: slider; y: 0 }
        },
        State {
            name: "closed"
            PropertyChanges { target: bg; opacity: 0 }
            PropertyChanges { target: slider; y: -slider.parent.height }
        }
    ]

    transitions: [
        Transition {
            from: "*"
            to: "*"
            NumberAnimation { target: slider; property: "y"; duration: 300; easing.type: Easing.InOutQuad }
        }
    ]
}
