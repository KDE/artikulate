/*
 *  SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import artikulate 1.0

Item {
    id: root
    width: button.width
    height: button.height

    /**
     * the display text on the button
     */
    property string text: i18n("Play")

    /**
     * the path to the sound file
     */
    property string fileUrl

    Player {
        id: playerBackend
        soundFileUrl: root.fileUrl
    }

    Button {
        id: button
        enabled: fileUrl !== ""
        text: root.text
        icon.name: "media-playback-start"

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
    states: [
        State {
            name: "playing"
            when: playerBackend.state === Player.PlayingState
            PropertyChanges {
                target: button
                icon.name: "media-playback-stop";
            }
        }
    ]
}
