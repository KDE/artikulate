/*
 *  Copyright 2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

import QtQuick 2.5
import QtQuick.Controls 1.2
import org.kde.kquickcontrolsaddons 2.0

Item {
    id: root
    property string iconName
    property string text
    property bool enabled: true
    property int iconSize: 12
    property int fontSize: 22
    signal clicked
    height: Math.max(10 + textLabel.height, 10 + root.iconSize)
    width: 30 + iconSize + (iconSize > 0 ? 10 : 0) + textLabel.paintedWidth

    SystemPalette {
        id: palette
        colorGroup: root.enabled ? SystemPalette.Active : SystemPalette.Disabled
    }

    Rectangle {
        id: bg
        border.color: palette.highlight
        border.width: 1
        radius: 4
        opacity: 0
        anchors.fill: parent
    }

    Item {
        id: padding

        anchors {
            fill: parent
            topMargin: 10
            rightMargin: 10
            bottomMargin: 10
            leftMargin: 10
        }

        Text {
            id: textLabel
            text: root.text
            font.pointSize: root.fontSize
            color: palette.buttonText
            anchors {
                left: parent.left
                leftMargin: 10
                verticalCenter: parent.verticalCenter
            }
            height: paintedHeight
            width: paintedWidth
        }
        QIconItem {
            id: iconItem
            visible: !!root.iconName
            anchors {
                left: textLabel.right
                leftMargin: 10
                verticalCenter: textLabel.verticalCenter
            }
            state: root.enabled ? QIconItem.DefaultState : QIconItem.DisabledState//TODO KF5.15 change to "enabled:"
            icon: root.iconName
            width: root.iconSize
            height: root.iconSize
        }

    }

    MouseArea {
        id: mouseArea
        enabled: root.enabled
        anchors.fill: parent
        hoverEnabled: true
        onClicked: root.clicked()
        onEntered: {
            bg.opacity = 100
        }
        onExited: {
            bg.opacity = 0
        }
        onPressed: {
            bg.color = palette.highlight
        }
        onReleased: {
            bg.color = palette.base
        }
    }
}
