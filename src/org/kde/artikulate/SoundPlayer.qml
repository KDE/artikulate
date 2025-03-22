// SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls
import QtMultimedia

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
    property alias source: player.source

    MediaPlayer {
        id: player
        audioOutput: AudioOutput {}
        onErrorOccurred: (error, errorString) => console.log(`playback error: ${errorString}`)
    }

    Button {
        id: button
        enabled: player.hasAudio
        text: root.text
        icon.name: player.playing ? "media-playback-stop" : "media-playback-start"
        onClicked: player.playing ? player.stop() : player.play()
    }
}
