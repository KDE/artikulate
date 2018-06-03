/*
 *  Copyright 2013-2018  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.0 as Kirigami
import QtQuick.Layouts 1.2
import artikulate 1.0

Kirigami.Page {
    id: root
    Layout.fillWidth: true

    title: i18n("Configure Profile")

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
