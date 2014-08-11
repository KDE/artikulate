/*
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

import QtQuick 2.1
import QtQuick.Controls 1.2
import artikulate 1.0

Item {
    id: root

    property Phrase phrase

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
            SoundRecorder {
                id: recorder
            }
            SoundPlayer {
                fileUrl: recorder.outputFileUrl
            }
        }
        Row {
            anchors { left: componentTitle.left; leftMargin: 30 }
            visible: recorder.outputFileUrl != ""

            ToolButton {
                anchors.verticalCenter: parent.verticalCenter
                iconName: "dialog-ok-apply"
                text: i18n("Replace existing recording")
                onClicked: {
                    recorder.storeToFile(phrase.soundFileOutputPath())
                }
            }
            ToolButton {
                anchors.verticalCenter: parent.verticalCenter
                iconName: "dialog-cancel"
                text: i18n("Dismiss")
                onClicked: {
                    recorder.clearBuffer()
                }
            }
        }
    }
}
