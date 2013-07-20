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

    height: phrase != null ? phraseText.height + audioControls.height + 20 : 0
    width: 500

    Column {
        spacing: 20
        Text {
            id: phraseText
            width: root.width - 120
            text: (phrase != null) ? phrase.text : ""
            wrapMode: Text.WordWrap
            font.pointSize: 24
            horizontalAlignment: Text.AlignHCenter
        }

        Row {
            id: audioControls
            spacing: 10
            anchors { horizontalCenter: parent.horizontalCenter }
            width: 3*48 + 20
            height: 48

            PlasmaComponents.ToolButton {
                property int soundState: phrase != null ? phrase.playbackSoundState : Phrase.StoppedState

                anchors.verticalCenter: audioControls.verticalCenter
                iconSource: "artikulate-media-playback-start"
                height: 48; width: 48
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

            PlasmaComponents.ToolButton {
                property bool recording: true

                anchors.verticalCenter: audioControls.verticalCenter
                iconSource: "artikulate-media-record"
                height: 48; width: 48

                onClicked: {
                    if (recording) {
                        phrase.startRecordUserSound();
                        recording = false;
                    }
                    else {
                        phrase.stopRecordUserSound();
                        recording = true;
                    }
                }
            }
            PlasmaComponents.ToolButton {
                property int userSoundState: (phrase != null) ? phrase.playbackUserSoundState : Phrase.StoppedState

                anchors.verticalCenter: audioControls.verticalCenter
                iconSource: "artikulate-media-playback-start"
                height: 48; width: 48
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
