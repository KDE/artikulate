/*
 *  Copyright 2012  Sebastian Gottfried <sebastiangottfried@web.de>
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

import QtQuick 1.1
import org.kde.locale 0.1 as Locale
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import artikulate 1.0

PlasmaComponents.Page {
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

    Image {
        id: imageLearner
        width: 120
        height: 120
        anchors {
            top: root.top
            right: root.right
            topMargin: 30
            rightMargin: 30
        }
        cache: false
        source: profile.imageUrl ? profile.imageUrl : "../images/user-identity.png"

        Connections {
            target: profile
            onImageUrlChanged: {
                imageLearner.source = "" // trigger reload
                if (profile.imageUrl) {
                    imageLearner.source = profile.imageUrl
                } else {
                    imageLearner.source = "../images/user-identity.png"
                }
            }
        }
    }

    PlasmaComponents.Label {
        anchors {
            right: imageLearner.left
            rightMargin: 30
            top: imageLearner.top
            topMargin: Math.floor(0.25*imageLearner.height)
        }
        height: paintedHeight
        font.pointSize: theme.defaultFont.pointSize * 1.2
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
        PlasmaComponents.ToolButton {
            iconSource: "document-edit"
            text: i18n("Edit")
            onClicked: root.state = "editor"
        }
        PlasmaComponents.ToolButton {
            iconSource: "edit-delete"
            text: i18n("Delete")
            enabled: profileManager.profileCount > 1
            onClicked: root.state = "deleteConfirmation"
        }
        PlasmaComponents.ToolButton {
            iconSource: "insert-image"
            text: i18n("Change Image")
            onClicked: profileManager.openImageFileDialog()
        }
    }

    Item {
        id: editorContainer
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
            property alias doneButtonIconSource: doneBtn.iconSource
            property alias doneButtonText: doneBtn.text

            width: parent.width
            height: editorContainer.height

            spacing: 15

            PlasmaComponents.TextField {
                id: nameTextField
                width: parent.width
                placeholderText: i18n("Name")
            }

            PlasmaComponents.Button {
                id: doneBtn
                anchors.horizontalCenter: parent.horizontalCenter
                text: i18n("Done")
                enabled: nameTextField.text !== ""
                iconSource: "dialog-ok"
                onClicked: {
                    root.profile.name = profileForm.name
                    if (root.profile.id === -1) {
                        profileManager.addProfile(profile)
                    }
                    else {
                        profileManager.sync(root.profile)
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

        anchors {
            top: editComponent.top
        }
        Column {
            width: parent.width
            height: parent.height
            spacing: 15

            PlasmaComponents.Label {
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
                PlasmaComponents.ToolButton {
                    iconSource: "edit-delete"
                    text: i18n("Delete")
                    onClicked: {
                        deletionRequest()
                    }
                }
                PlasmaComponents.ToolButton {
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
