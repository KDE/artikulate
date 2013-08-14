/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
    width: 500
    Item {
        width: phraseText.width + 30
        height: phraseText.height + 30
        anchors { horizontalCenter: parent.horizontalCenter }
        anchors { top: parent.top }
        Image {
            width: 30
            height: 30
            anchors.horizontalCenter: parent.left
            anchors.verticalCenter: parent.top
            source: "../images/language.png"
        }
        Image {
            width: 30
            height: 30
            anchors.horizontalCenter: parent.right
            anchors.verticalCenter: parent.bottom
            source: "../images/language.png"
        }
        Rectangle {
            width: phraseText.width
            height: phraseText.height
            anchors { horizontalCenter: parent.horizontalCenter }
            anchors { verticalCenter: parent.verticalCenter }
                
            gradient: Gradient {
                GradientStop { position: 1.0; color: "#F2F2F2" }
                GradientStop { position: 0.1; color: "#E3E3E3" }
            }
            Text {
                id: phraseText
                width: ((phrase.text.length * 19) > (root.width-120)) ? root.width-120 : phrase.text.length * 19
                text: (phrase != null) ? phrase.text : ""
                wrapMode: Text.WordWrap
                font.pointSize: ((phrase.text.length * 19) > (root.width-120)*5) ? 20 :24
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
    Row {
        id: audioControls
        spacing: 400
        anchors { horizontalCenter: parent.horizontalCenter }
        anchors { bottom: parent.bottom }
        width: 90*2 + 400
        height: 90
        Item {
            width: 90
            height: parent.height
            //anchors.verticalCenter: audioControls.left
            Image {
                width: parent.width
                height: parent.height
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                source: "../images/language.png"
            }

            PlasmaComponents.ToolButton {
                property int soundState: phrase != null ? phrase.playbackSoundState : Phrase.StoppedState
                x: 45; y: 40;
                //anchors.verticalCenter: parent.bottom
                //anchors.horizontalCenter: parent.right
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
            height: parent.height
            //anchors.verticalCenter: audioControls.left
            Image {
                width: parent.width
                height: parent.height
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                source: "../images/user-identity.png"
            }

            PlasmaComponents.ToolButton {
                property bool isRecording: false
                anchors.horizontalCenter: parent.left
                y: 40;
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

