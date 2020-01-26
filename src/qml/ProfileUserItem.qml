/*
 *  Copyright 2012       Sebastian Gottfried <sebastiangottfried@web.de>
 *  Copyright 2013-2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.1
import QtQuick.Controls 1.2
import artikulate 1.0

Item {
    id: root
    anchors.fill: parent

    property Learner profile: null
    state: "info"

    function update() {
        if (profile) {
            var isNewProfile = root.profile === null
            profileForm.name = profile.name
            deleteConfirmationLabel.name = profile.name
            state = isNewProfile ? "editor" : "info"
        }
    }

    signal deletionRequest();

    onProfileChanged: update()

    ProfileUserImageItem {
        id: imageLearner
        height: 120
        width: height
        profile: root.profile
        anchors {
            top: root.top
            right: root.right
            topMargin: 30
            leftMargin: 30
        }
    }

    Label {
        anchors {
            top: imageLearner.top
            topMargin: Math.floor(0.25 * imageLearner.height)
            leftMargin: 30
            left: parent.left
        }
        height: paintedHeight
        font.pointSize: theme.fontPointSize * 1.2
        text: root.profile != null ? root.profile.name : ""
    }

    Row {
        id: editComponent
        spacing: 10
        visible: root.state == "info"
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: imageLearner.bottom
            topMargin: 30
        }
        Button {
            iconName: "document-edit"
            text: i18n("Edit")
            onClicked: root.state = "editor"
        }
        Button {
            iconName: "edit-delete"
            text: i18n("Delete")
            enabled: g_profileManager.profileCount > 1
            onClicked: root.state = "deleteConfirmation"
        }
        Button {
            iconName: "insert-image"
            text: i18n("Change Image")
            onClicked: g_profileManager.openImageFileDialog()
        }
        ToolButton {
            iconName: "edit-clear"
            text: i18n("Clear Image")
            onClicked: {
                g_profileManager.activeProfile.clearImage()
            }
        }
    }

    Item {
        id: editorContainer
        visible: false
        width: parent.width - 40
        height: editorContainer.height
        anchors {
            horizontalCenter: parent.horizontalCenter
            top: imageLearner.bottom
            topMargin: 30
        }

        Column {
            id: profileForm

            property alias name: nameTextField.text
            property alias doneButtonIconSource: doneBtn.iconName
            property alias doneButtonText: doneBtn.text

            width: parent.width
            height: editorContainer.height

            spacing: 15

            TextField {
                id: nameTextField
                width: parent.width
                placeholderText: i18n("Name")
            }

            Button {
                id: doneBtn
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18n("Done")
                enabled: nameTextField.text !== ""
                iconName: "dialog-ok"
                onClicked: {
                    root.profile.name = profileForm.name
                    if (root.profile.id === -1) {
                        g_profileManager.addProfile(profile)
                    }
                    else {
                        g_profileManager.sync(root.profile)
                    }
                    root.update()
                    root.state = "info"
                }
            }
        }
    }

    Item {
        id: deleteConfirmationContainer
        width: parent.width - 40
        height: editorContainer.height
        visible: false

        anchors {
            top: editComponent.top
            leftMargin: 20
            left: parent.left
        }
        Column {
            width: parent.width
            height: parent.height
            spacing: 15

            Label {
                property string name
                id: deleteConfirmationLabel
                width: parent.width
                text: i18n("Do you really want to delete this identity \"<b>%1</b>\"?", name)
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
            }
            Row {
                spacing: 10
                anchors.horizontalCenter: parent.horizontalCenter
                Button {
                    iconName: "edit-delete"
                    text: i18n("Delete")
                    onClicked: {
                        deletionRequest()
                    }
                }
                Button {
                    iconName: "dialog-cancel"
                    text: i18n("Cancel")
                    onClicked: root.state = "info"
                }
            }
        }
    }

    states: [
        State {
            name: "info"
            PropertyChanges {
                target: editorContainer
                visible: false
            }
            PropertyChanges {
                target: deleteConfirmationContainer
                visible: false
            }
        },
        State {
            name: "editor"
            PropertyChanges {
                target: editorContainer
                visible: true
            }
            PropertyChanges {
                target: deleteConfirmationContainer
                visible: false
            }
        },
        State {
            name: "deleteConfirmation"
            PropertyChanges {
                target: editorContainer
                visible: false
            }
            PropertyChanges {
                target: deleteConfirmationContainer
                visible: true
            }
        }
    ]
}
