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
import artikulate 1.0
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: root

    property Phrase phrase
    height: 320
    width: 400
    Item {
        id: phraseDisplay
        width: Math.max(400, parent.width * 0.5)
        height: Math.max(150, phraseText.lineCount * 30)
        anchors { horizontalCenter: parent.horizontalCenter; top: parent.top }

        // phrase text element
        Text {
            id: phraseText
            objectName: "phraseText"
            anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }
            width: parent.width - 64
            text: (phrase != null) ? phrase.text : ""
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
        }

        // quotation marks
        Image {
            width: 32
            height: 32
            anchors.horizontalCenter: parent.left
            anchors.verticalCenter: parent.top
            source: "../images/quotationmark-left.png"
        }
        Image {
            width: 32
            height: 32
            anchors.horizontalCenter: parent.right
            anchors.verticalCenter: parent.bottom
            source: "../images/quotationmark-right.png"
        }

    }

    Row {
        id: audioControls
        spacing: 400
        anchors { horizontalCenter: parent.horizontalCenter }
        anchors { top: phraseDisplay.bottom; topMargin: 50 }
        width: 90*2 + 400

        Item {
            width: 90
            height: imageLanguage.height + textNativeSpeaker.height + 10

            Image {
                id: imageLanguage
                height: 90
                width: 90
                anchors { horizontalCenter: parent.horizontalCenter }
                source: "../images/language.png"
            }
            Text {
                id: textNativeSpeaker
                text: i18n("Native Speaker")
                anchors { top: imageLanguage.bottom; topMargin: 10; horizontalCenter: parent.horizontalCenter }
                font.pointSize: 14
                color: "gray"
            }

            PlasmaComponents.ToolButton {
                property int soundState: phrase != null ? phrase.playbackSoundState : Phrase.StoppedState
                x: 45; y: 40;

                iconSource: "artikulate-media-playback-start"
                height: 50; width: 50
                enabled: phrase != null ? phrase.isSound : false

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
        }
        Item {
            width: 90
            height: imageLearner.height + textLearner.height + 10

            Image {
                id: imageLearner
                width: 90
                height: 90
                source: "../images/user-identity.png"
            }
            Text {
                id: textLearner
                anchors { top: imageLearner.bottom; topMargin: 10; horizontalCenter: imageLearner.horizontalCenter }
                text: i18n("Learner (You)")
                font.pointSize: 14
                color: "gray"
            }

            PlasmaComponents.ToolButton {
                property bool isRecording: false
                x: -20
                y: 40
                iconSource: "artikulate-media-record"
                height: 52; width: 52

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
                x: 50
                y: 40;
                anchors.horizontalCenter: parent.right
                iconSource: "artikulate-media-playback-start"
                height: 52; width: 52
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
}

