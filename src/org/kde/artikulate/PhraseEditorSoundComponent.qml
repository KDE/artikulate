// SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia
import org.kde.artikulate

RowLayout {
    id: root

    property Phrase phrase

    MediaPlayer {
        id: storedRecordingPlayer
        audioOutput: AudioOutput {}
        onErrorOccurred: (error, errorString) => console.log(`playback error: ${errorString}`)
    }
    MediaPlayer {
        id: newRecordingPlayer
        audioOutput: AudioOutput {}
        onErrorOccurred: (error, errorString) => console.log(`playback error: ${errorString}`)
    }

    Button {
        id: playButton
        enabled: phrase.sound !== undefined
        text: i18nc("@action:button", "Play stored recording")
        icon.name: storedRecordingPlayer.playing ? "media-playback-stop" : "media-playback-start"
        onClicked: {
            if (storedRecordingPlayer.playing) {
                storedRecordingPlayer.stop()
            } else {
                storedRecordingPlayer.source = phrase.sound
                storedRecordingPlayer.play()
            }
        }
    }

    Item { // space
        Layout.preferredWidth: 20
    }

    Button {
        id: recordButton
        checkable: false
        icon.name: recorder.recorderState === MediaRecorder.RecordingState ? "media-playback-stop" :  "media-record"
        text: i18nc("@action:button", "Record new")
        onClicked: {
            if (recorder.recorderState === MediaRecorder.RecordingState) {
                recorder.stop()
                TemporaryRecordingFile.soundAvailable = true
            }
            else {
                TemporaryRecordingFile.iterate()
                recorder.record()
            }
        }
    }
    Button {
        id: playNewButton
        enabled: TemporaryRecordingFile.path
        text: i18nc("@action:button", "Play new recording")
        icon.name: newRecordingPlayer.playing ? "media-playback-stop" : "media-playback-start"
        onClicked: {
            if (newRecordingPlayer.playing) {
                newRecordingPlayer.stop()
            } else {
                newRecordingPlayer.source = TemporaryRecordingFile.path
                newRecordingPlayer.play()
            }
        }
    }
    Button {
        id: applyButton
        checkable: false
        enabled: TemporaryRecordingFile.soundAvailable
        icon.name: "document-replace"
        text: i18nc("@action:button", "Take new recording")
        onClicked: {
            TemporaryRecordingFile.saveAs(root.phrase.soundFileOutputPath())
            root.phrase.markSoundRecorded()
        }
    }

    CaptureSession {
        audioInput: AudioInput {
        }
        recorder: MediaRecorder {
            id: recorder
            mediaFormat {
                audioCodec: MediaFormat.AudioCodec.Vorbis
            }
            outputLocation: TemporaryRecordingFile.path
        }
    }
}
