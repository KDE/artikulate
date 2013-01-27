/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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
    width: 100
    height: 200

    property PhraseModel phraseModel
    property Unit unit

    Component {
        id: itemDelegate

        Item {
            height: 30
            Row {
                PlasmaComponents.ToolButton {
                    iconSource: "media-playback-start"
                    property Phrase phrase: model.dataRole
                    property int soundState: phrase.playbackSoundState
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
                    iconSource: "media-record"
                    property Phrase phrase: model.dataRole
                    onClicked: {
                        //TODO
                    }
                }
                PlasmaComponents.ToolButton {
                    iconSource: "media-playback-start"
                    property Phrase phrase: model.dataRole
                    property int userSoundState: phrase.playbackUserSoundState
                    onClicked: {
                       if (userSoundState == Phrase.PlayingState) {
                            phrase.stopUserSound();
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
                    text: model.text
                }
            }
        }
    }

    ListView {
        id: phraseList
        anchors.fill: parent

        model: root.phraseModel
        delegate: itemDelegate
    }
}
