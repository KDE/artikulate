// SPDX-FileCopyrightText: 2014-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls
import org.kde.artikulate

Item {
    id: root
    width: button.width
    height: button.height

    /**
     * the display text on the button
     */
    property string text: i18n("Record")

    /**
     * the path to the sound file
     */
    readonly property string outputFileUrl: recorderBackend.recordingFile

    function storeToFile(filePath) {
        recorderBackend.storeToFile(filePath);
        phrase.setSoundFileUrl()
    }

    function clearBuffer() {
        recorderBackend.clearBuffer()
    }

    Recorder {
        id: recorderBackend
    }

    Button {
        id: button
        checkable: false
        icon.name: "media-record"
        text: root.text

        onClicked: {
            if (recorderBackend.state === Recorder.RecordingState) {
                console.log("try to stop recording");
                recorderBackend.stop();
                return;
            }
            if (recorderBackend.state === Recorder.StoppedState) {
                console.log("SoundRecorder: start capture");
                recorderBackend.startCapture();
                return;
            }
        }
    }
    states: [
        State {
            name: "recording"
            when: recorderBackend.state === Recorder.RecordingState
            PropertyChanges {
                target: button
                icon.name: "media-playback-stop"
            }
        }
    ]
}
