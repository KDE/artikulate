// SPDX-FileCopyrightText: 2014-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
    property string text: i18n("Record")

    CaptureSession {
        audioInput: AudioInput {
        }
        recorder: MediaRecorder {
            id: recorder
            mediaFormat {
                audioCodec: MediaFormat.AudioCodec.Vorbis
            }
            outputLocation: TrainingSoundFile.path
        }
    }
    Button {
        id: button
        checkable: false
        icon.name: recorder.recorderState === MediaRecorder.RecordingState ? "media-playback-stop" :  "media-record"
        text: root.text
        onClicked: {
            if (recorder.recorderState === MediaRecorder.RecordingState) {
                recorder.stop()
                TrainingSoundFile.soundAvailable = true
            }
            else {
                TrainingSoundFile.iterate()
                recorder.record()
            }
        }
    }
}
