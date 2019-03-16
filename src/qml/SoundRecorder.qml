/*
 *  Copyright 2014-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
