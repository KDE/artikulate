/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *  Copyright 2013       Magdalena Konkiewicz <konkiewicz.m@gmail.com>
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

import QtQuick 2.5
import QtQuick.Shapes 1.10
import QtQuick.Controls 2.3 as QQC2
import org.kde.kirigami 2.7 as Kirigami
import artikulate 1.0

Kirigami.Page {
    id: root

    readonly property color colorTask: "#1dbf4e"
    readonly property color colorAnswer: "#7e48a5"

    background: Rectangle {
        color: "#ffffff"
    }

    title: {
        var titleString = "";
        if (g_trainingSession.unit === null) {
            titleString += i18n("Category: no category selected");
        }
        else {
            titleString += i18n("Category: ") + g_trainingSession.unit.title
        }
        if (g_trainingSession.unit !== null && g_trainingSession.course !== null) {
            titleString += " / " + g_trainingSession.course.i18nTitle
        }
        return titleString
    }
    actions {
        main: Kirigami.Action {
            text: i18n("Next")
            tooltip: i18n("Mark current phrase as completed and proceed with next one.")
            iconName: "dialog-ok"
            onTriggered: g_trainingSession.accept()
        }
        right: Kirigami.Action {
            text: i18n("Skip")
            tooltip: i18n("Skip current phrase and proceed with next one.")
            iconName: "go-next"
            enabled: g_trainingSession.hasNext
            onTriggered: g_trainingSession.skip()
        }
    }
    Rectangle {
        id: trainingTextRect
        width: Math.min(0.7 * parent.width, parent.width - 80)
        height: Math.max(200, phraseText.implicitHeight)
        anchors {
            left: parent.left
            top: parent.top
            leftMargin: 20
            topMargin: 20
        }
        color: root.colorTask

        Shape {
            id: taskTriangle
            width: 50
            height: 40
            anchors {
                top: parent.bottom
                horizontalCenter: parent.horizontalCenter
                horizontalCenterOffset: parent.width / 10
            }
            ShapePath {
                fillColor: colorTask
                strokeColor: colorTask
                PathLine { x: 0; y: 0 }
                PathLine { x: taskTriangle.width; y: taskTriangle.height }
                PathLine { x: taskTriangle.width; y: 0 }
            }
        }

        QQC2.TextArea {
            id: phraseText
            anchors.fill: parent
            objectName: "phraseText"
            text: (g_trainingSession.phrase !== null) ? g_trainingSession.phrase.text : ""
            font.pointSize: 24
            wrapMode: Text.WordWrap
            readOnly: true
            background: Item {}
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: TextEdit.AlignVCenter
        }
        SoundPlayer {
            id: buttonNativePlay
            anchors {
                top: taskTriangle.bottom
                topMargin: 20
                horizontalCenter: taskTriangle.right
            }
            text: i18n("Play original")
            fileUrl: g_trainingSession.phrase === null ? "" : g_trainingSession.phrase.soundFileUrl
        }
    }

    Rectangle {
        id: trainingUserRect
        width: 200
        height: 0.65 * width
        anchors {
            right: parent.right
            top: trainingTextRect.bottom
            rightMargin: 20
            topMargin: 154
        }
        color: root.colorAnswer

        Shape {
            id: answerTriangle
            width: 50
            height: 40
            anchors {
                bottom: parent.top
                horizontalCenter: parent.horizontalCenter
                horizontalCenterOffset: -parent.width / 10
            }
            ShapePath {
                fillColor: root.colorAnswer
                strokeColor: root.colorAnswer
                PathLine { x: 0; y: 0 }
                PathLine { x: 0; y: taskTriangle.height }
                PathLine { x: taskTriangle.width; y: taskTriangle.height }
            }
        }
        SoundRecorder {
            id: recorder
            anchors {
                bottom: answerTriangle.top
                bottomMargin: 20
                horizontalCenter: answerTriangle.left
            }
            text: i18n("Record yourself")
        }

        SoundPlayer {
            id: player
            anchors {
                centerIn: parent
            }
            text: i18n("Play yourself")
            fileUrl: recorder.outputFileUrl
        }
    }
}
