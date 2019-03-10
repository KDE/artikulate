/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import QtQuick.Controls 2.1 as QQC2
import org.kde.kirigami 2.0 as Kirigami
import artikulate 1.0

Item {
    id: root
    width: 32
    height: width

    property string fileUrl

    signal stopped()

    Connections {
        target: playerBackend
        onStateChanged: {
            // set next possible action icon
            if (playerBackend.state === Player.PlayingState) {
                statusIcon.source = "media-playback-stop";
                return
            }
            if (playerBackend.state === Player.StoppedState) {
                statusIcon.source = "media-playback-start";
                return
            }
        }
    }

    Player {
        id: playerBackend
        soundFileUrl: root.fileUrl
    }

    QQC2.RoundButton {
        enabled: fileUrl !== ""
        Kirigami.Icon {
            id: statusIcon
            source: "media-playback-start"
            width: root.width
            height: width
            anchors.centerIn: parent
        }

        onClicked: {
            if (playerBackend.state === Player.PlayingState) {
                playerBackend.stop();
                return;
            }
            if (playerBackend.state === Player.StoppedState) {
                playerBackend.playback();
                return;
            }
        }
    }
}
