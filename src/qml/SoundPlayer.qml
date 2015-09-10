/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import org.kde.kquickcontrolsaddons 2.0
import artikulate 1.0

Item {
    id: root

    property string fileUrl

    signal stopped()

    width: theme.mediumIconSize
    height: theme.mediumIconSize

    Player {
        id: playerBackend
        soundFileUrl: root.fileUrl
    }

    ToolButton {
        id: playButton
        height: Math.max(root.width, root.height)
        width: Math.max(root.width, root.height)
        enabled: fileUrl != ""
        QIconItem {
            id: iconItem
            anchors.fill: parent
            icon: "media-playback-start"
        }

        onClicked: {
            if (playerBackend.state == Player.PlayingState) {
                playerBackend.stop();
                return;
            }
            if (playerBackend.state == Player.StoppedState) {
                playerBackend.playback();
                return;
            }
        }
        Connections {
            target: playerBackend
            onStateChanged: {
                // set next possible action icon
                if (playerBackend.state == Player.PlayingState) {
                    iconItem.icon = "media-playback-stop";
                    return
                }
                if (playerBackend.state == Player.StoppedState) {
                    iconItem.icon = "media-playback-start";
                    return
                }
            }
        }
    }
}
