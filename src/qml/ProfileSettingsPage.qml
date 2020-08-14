/*
 *  SPDX-FileCopyrightText: 2013-2018 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.1
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.7 as Kirigami
import QtQuick.Layouts 1.2
import artikulate 1.0

Kirigami.Page {
    id: root
    Layout.fillWidth: true

    title: i18n("Configure Profile")
    background: Rectangle {
        color: "#ffffff"
    }

    readonly property Learner profile: g_profileManager.activeProfile

    signal deletionRequest()

    actions {
        main: Kirigami.Action {
            id: editorAction
            text: i18n("Edit")
            iconName: "document-edit"
            checkable: true
        }
    }

    actions.contextualActions: [
        Kirigami.Action {
            iconName: "insert-image"
            text: i18n("Select Image")
            onTriggered: {
                g_profileManager.openImageFileDialog()
            }
        },
        Kirigami.Action {
            iconName: "edit-clear"
            text: i18n("Clear Image")
            onTriggered: {
                g_profileManager.activeProfile.clearImage()
            }
        },
        Kirigami.Action {
            text: i18n("Delete User")
            iconName: "edit-delete"
            enabled: root.state == "editor" && g_profileManager.profileCount > 1 //TODO handle this situation gracefully by allowing removal of last profile
            onTriggered: deletionRequest()
        }
    ]


    // image
    MouseArea {
        id: image
        height: 120
        width: height

        anchors {
            top: parent.top
            right: parent.right
            topMargin: 30
            leftMargin: 30
        }

        ProfileUserImageItem {
            anchors.centerIn: parent
            profile: root.profile
        }

        onClicked: g_profileManager.openImageFileDialog()
    }

    Text {
        id: learnerNameText
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: image.bottom
            topMargin: 30
        }
        height: paintedHeight
        font.pointSize: theme.fontPointSize * 1.2
        text: root.profile != null ? root.profile.name : ""
    }

    QQC2.TextField {
        id: learnerNameTextEdit
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: image.bottom
            topMargin: 30
        }
        visible: false
        width: parent.width
        placeholderText: i18n("Name")
        text: learnerNameText.text
        onTextChanged: {
            root.profile.name = text
            g_profileManager.sync(root.profile)
        }
    }

    states: [
        State {
            name: "editor"
            when: editorAction.checked
            PropertyChanges {
                target: learnerNameTextEdit
                visible: true
            }
        }
    ]
}
