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
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import artikulate 1.0

Item {
    id: root

    property Phrase phrase
    property bool recordingBuffered: false

    width: mediaController.width
    height: mediaController.height

    Column {
        id: mediaController

        Text {
            id: componentTitle
            text: i18n("Native Speaker Recording")
            font.pointSize: 14;
        }

        Row {
            anchors { left: componentTitle.left; leftMargin: 30 }

            height: 30
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: i18n("Existing Recording:")
            }
            SoundPlayer {
                fileUrl: root.phrase == null ? "" : phrase.soundFileUrl
            }
        }
        Row {
            anchors { left: componentTitle.left; leftMargin: 30 }
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: i18n("Create New Recording:")
            }
            PlasmaComponents.ToolButton {
                property int recordingState: (root.phrase != null) ? root.phrase.recordingState : Phrase.NotRecordingState
                property bool isRecording: false

                anchors.verticalCenter: parent.verticalCenter
                iconSource: "artikulate-media-record"

                onClicked: {
                    if (!isRecording) { // hence, start recording
                        isRecording = true
                        phrase.startRecordNativeSound();
                    }
                    else { // hence, stop recording
                        isRecording = false;
                        phrase.stopRecordNativeSound();
                        recordingBuffered = true;
                    }
                }
                onRecordingStateChanged: {
                    // set next possible action icon
                    if (recordingState == Phrase.CurrentlyRecordingState) {
                        iconSource = "artikulate-media-record-active";
                        console.log("change state to active")
                    }
                    if (recordingState == Phrase.NotRecordingState) {
                        iconSource = "artikulate-media-record";
                        console.log("change state to stop")
                    }
                }
            }
            SoundPlayer {
                fileUrl: root.phrase == null ? "" : phrase.soundRecordingBufferUrl
            }
        }
        Row {
            anchors { left: componentTitle.left; leftMargin: 30 }
            visible: recordingBuffered

            PlasmaComponents.ToolButton {
                anchors.verticalCenter: parent.verticalCenter
                iconSource: "dialog-ok-apply"
                text: i18n("Replace existing recording")
                onClicked: {
                    phrase.applyRecordedNativeSound();
                    recordingBuffered = false
                }
            }
            PlasmaComponents.ToolButton {
                anchors.verticalCenter: parent.verticalCenter
                iconSource: "dialog-cancel"
                text: i18n("Dismiss")
                onClicked: {
                    recordingBuffered = false
                }
            }
        }
    }
}
