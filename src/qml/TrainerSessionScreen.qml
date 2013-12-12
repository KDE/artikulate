/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *  Copyright 2013  Magdalena Konkiewicz <konkiewicz.m@gmail.com>
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

import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import artikulate 1.0

Item {
    id: root

    property Unit unit
    property TrainingSession session
    property Phrase phrase: session.currentPhrase

    // text bubble
    Item {
        id: phraseBubble
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: 30
        }
        width: Math.floor(parent.width * 0.7)
        height: Math.floor(parent.width * 0.2)

        Rectangle {
            id: phraseBubbleRect
            anchors.fill: parent
            color: "white"
            border.color: "black"
            border.width: 5
            radius: 30

            // phrase text element
            Text {
                id: phraseText
                objectName: "phraseText"
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    verticalCenter: parent.verticalCenter
                }
                width: parent.width - 64
                text: (phrase != null) ? phrase.text : ""
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
            }
        }

        Image {
            id: hook
            anchors {
                top: phraseBubbleRect.bottom
                topMargin: -2
                left: phraseBubbleRect.left
                leftMargin: 170
            }
            width: 120
            height: 60
            source: "../images/speak-hook.png"
        }
    }

    Item {
        id: audioControls
        anchors {
            top: phraseBubble.bottom
            left: phraseBubble.left
            topMargin: 30
            leftMargin: 70
        }
        width: 128
        height: buttonNativePlay.height + textNativeSpeaker.height + 10

        PlasmaComponents.ToolButton {
            id: buttonNativePlay
            property int soundState: phrase != null ? phrase.playbackSoundState : Phrase.StoppedState
            height: 96
            width: 96
            enabled: phrase != null ? phrase.isSound : false
            iconSource: "artikulate-media-playback-start"

            onClicked: {
                if (soundState == Phrase.PlayingState) {
                    phrase.stopSound();
                }
                if (soundState == Phrase.StoppedState) {
                    phrase.playbackSound();
                }
            }
            onSoundStateChanged: {
                // set next possible action icon
                if (soundState == Phrase.PlayingState) {
                    iconSource = "artikulate-media-playback-stop";
                }
                if (soundState == Phrase.StoppedState) {
                    iconSource = "artikulate-media-playback-start";
                }
            }
        }

        Text {
            id: textNativeSpeaker
            text: i18n("Native Speaker")
            anchors {
                top: buttonNativePlay.bottom
                topMargin: 10
                horizontalCenter: buttonNativePlay.horizontalCenter
            }
            font.pointSize: 14
            color: "gray"
        }
    }

    Item {
        id: learnerBubble
        anchors {
            left: phraseBubble.left
            top: phraseBubble.bottom
            leftMargin: Math.floor(phraseBubble.width * 2/3)
            topMargin: 40
        }
        width: Math.floor(parent.width * 0.5)
        height: 120

        Rectangle {
            id: learnerBubbleRect
            anchors.fill: parent
            color: "white"
            border.color: "black"
            border.width: 5
            radius: 30

            Row {
                width: 2*96 + 30
                height: 96
                anchors.centerIn: parent
                spacing: 30

                PlasmaComponents.ToolButton {
                    property bool isRecording: false
                    iconSource: "artikulate-media-record"
                    height: 96
                    width: 96

                    onClicked: {
                        if (!isRecording) {
                            phrase.startRecordUserSound()
                            isRecording = true
                        }
                        else {
                            phrase.stopRecordUserSound()
                            isRecording = false
                        }
                    }
                    onIsRecordingChanged: {
                        if (isRecording) {
                            iconSource = "artikulate-media-record-active";
                        }
                        else {
                            iconSource = "artikulate-media-record";
                        }
                    }
                }
                PlasmaComponents.ToolButton {
                    property int userSoundState: (phrase != null) ? phrase.playbackUserSoundState : Phrase.StoppedState
                    iconSource: "artikulate-media-playback-start"
                    height: 96
                    width: 96
                    enabled: (phrase != null) ? phrase.isUserSound : false

                    onClicked: {
                        if (userSoundState == Phrase.PlayingState) {
                            phrase.stopPlaybackUserSound();
                        }
                        if (userSoundState == Phrase.StoppedState) {
                            phrase.playbackUserSound();
                        }
                    }
                    onUserSoundStateChanged: {
                        // set next possible action icon
                        if (userSoundState == Phrase.PlayingState) {
                            iconSource = "artikulate-media-playback-stop";
                        }
                        if (userSoundState == Phrase.StoppedState) {
                            iconSource = "artikulate-media-playback-start";
                        }
                    }
                }
            }
        }

        Image {
            id: learnerBubbleHook
            anchors {
                top: learnerBubble.bottom
                topMargin: -2
                left: learnerBubble.left
                leftMargin: 140
            }
            width: 120
            height: 60
            source: "../images/speak-hook.png"
        }

        Item {
            anchors {
                top: learnerBubble.bottom
                left: learnerBubble.left
                topMargin: 30
                leftMargin: 40
            }
            width: 128
            height: imageLearner.height + textLearner.height + 10

            Image {
                id: imageLearner
                width: 128
                height: 128
                source: "../images/user-identity.png"
            }
            Text {
                id: textLearner
                anchors {
                    top: imageLearner.bottom
                    topMargin: 10
                    horizontalCenter: imageLearner.horizontalCenter
                }
                text: i18n("Learner (You)")
                font.pointSize: 14
                color: "gray"
            }
        }
    }


    Column {
        id: trainingControls
        anchors {
            right: learnerBubble.right
            verticalCenter: phraseBubble.verticalCenter
        }
        width: Math.max(buttonApply.width, buttonStepOver.width)
        PlasmaComponents.ToolButton {
            id: buttonApply
            height: 96
            iconSource: "dialog-ok-apply"
            onClicked: {
                session.next(TrainingSession.Complete)
            }
        }
        PlasmaComponents.ToolButton {
            id: buttonStepOver
            height: 96
            iconSource: "object-rotate-left"
            onClicked: {
                session.next(TrainingSession.Incomplete)
            }
        }
    }
}
