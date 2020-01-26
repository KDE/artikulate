/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.5
import QtQuick.Controls 2.3
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

            height: 48
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
                icon.name: "dialog-ok-apply"
                text: i18n("Replace Existing Recording")
                onClicked: {
                    recorder.storeToFile(phrase.soundFileOutputPath())
                }
            }
            ToolButton {
                anchors.verticalCenter: parent.verticalCenter
                icon.name: "dialog-cancel"
                text: i18n("Dismiss")
                onClicked: {
                    recorder.clearBuffer()
                }
            }
        }
    }
}
