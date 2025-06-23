// SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-FileCopyrightText: 2013 Magdalena Konkiewicz <konkiewicz.m@gmail.com>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Shapes
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.artikulate

Kirigami.Page {
    id: root

    readonly property color colorTask: "#1dbf4e"
    readonly property color colorAnswer: "#7e48a5"

    Connections {
        target: TrainingSession
        function onPhraseChanged() {
            TemporaryRecordingFile.iterate()
        }
    }

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
        Kirigami.Theme.colorSet: Kirigami.Theme.View
    }

    title: {
        var titleString = "";
        if (TrainingSession.unit === null) {
            titleString += i18n("Category: no category selected");
        }
        else {
            titleString += i18n("Category: %1", TrainingSession.unit.title)
        }
        if (TrainingSession.unit !== null && TrainingSession.course !== null) {
            titleString += " / " + TrainingSession.course.i18nTitle
        }
        return titleString
    }
    actions: [
        // main
        Kirigami.Action {
            text: i18n("Next")
            tooltip: i18n("Mark current phrase as completed and proceed with next one.")
            icon.name: "dialog-ok"
            onTriggered: TrainingSession.accept()
        },
        // right
        Kirigami.Action {
            text: i18n("Skip")
            tooltip: i18n("Skip current phrase and proceed with next one.")
            icon.name: "go-next"
            enabled: TrainingSession.hasNext
            onTriggered: TrainingSession.skip()
        }
    ]
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
                fillColor: root.colorTask
                strokeColor: root.colorTask
                PathLine { x: 0; y: 0 }
                PathLine { x: taskTriangle.width; y: taskTriangle.height }
                PathLine { x: taskTriangle.width; y: 0 }
            }
        }

        QQC2.TextArea {
            id: phraseText
            anchors.fill: parent
            objectName: "phraseText"
            text: (TrainingSession.phrase !== null) ? TrainingSession.phrase.text : ""
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
            source: TrainingSession.phrase === null ? "" : TrainingSession.phrase.sound
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
            enabled: TemporaryRecordingFile.soundAvailable
            source: TemporaryRecordingFile.path
        }
    }
}
