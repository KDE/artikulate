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

    height: phrase != null ? phraseLine.height : 0
    width: 500

    Row {
        id: phraseLine

        PlasmaComponents.ToolButton {
            property int soundState: phrase != null ? phrase.playbackSoundState : Phrase.StoppedState

            anchors.verticalCenter: phraseLine.verticalCenter
            iconSource: "media-playback-start"
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
                    iconSource = "media-playback-stop";
                }
                if (soundState == Phrase.StoppedState) {
                    iconSource = "media-playback-start";
                }
            }
        }

        Item {
            width: 30
            height: 20
        }

        PlasmaComponents.ToolButton {
            property bool recording: true

            anchors.verticalCenter: phraseLine.verticalCenter
            iconSource: "media-record"

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

            anchors.verticalCenter: phraseLine.verticalCenter
            iconSource: "media-playback-start"
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
                    iconSource = "media-playback-stop";
                }
                if (userSoundState == Phrase.StoppedState) {
                    iconSource = "media-playback-start";
                }
            }
        }
        Text {
            width: root.width - 120
            anchors.verticalCenter: phraseLine.verticalCenter
            text: (phrase != null) ? phrase.text : ""
            wrapMode: Text.WordWrap
        }
    }
}
