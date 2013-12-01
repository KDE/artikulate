/*
 *  Copyright 2012  Sebastian Gottfried <sebastiangottfried@web.de>
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
import org.kde.locale 0.1 as Locale
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import artikulate 1.0

Item {
    id: root

    property Learner profile: null
    property ProfileManager manager: profileManager
    state: "init"

    function update() {
        if (profile) {
            var isNewProfile = root.profile === null
            profileForm.name = profile.name
            deleteConfirmationLabel.name = profile.name
            state = isNewProfile ? "editor": "info"
        }
    }

    signal deletionRequest();

    onProfileChanged: update()

    Locale.Locale {
        id: locale
    }

    Item {
        id: infoContainer
        width: parent.width
        height: childrenRect.height
        anchors.centerIn: parent

        Column {
            width: parent.width
            height: childrenRect.height
            spacing: 10

            PlasmaComponents.Label {
                id: languageTitleLabel
                height: paintedHeight
                font.pointSize: 1.5 * theme.defaultFont.pointSize
                text : i18n("Favorite Languages")
            }

            ListView {
                id: languageList
                width: parent.width
                height: Math.min(Math.floor(root.height/2), languageList.count * 30)
                clip: true
                model: LearningGoalModel {
                    profileManager: root.manager
                }
                delegate : Row {
                    spacing : 10
                    property LearningGoal goal: model.dataRole
                    width: languageList.width - 10
                    PlasmaComponents.ToolButton {
                        id: goalSelector
                        Connections {
                            target: root.profile
                            onGoalsChanged: {
                                goalSelector.checked = root.profile.hasGoal(goal)
                            }
                        }
                        Connections {
                            target: root
                            onProfileChanged: {
                                if (root.profile != null) {
                                    goalSelector.checked = root.profile.hasGoal(goal)
                                }
                            }
                        }
                        anchors.verticalCenter: parent.verticalCenter
                        iconSource: "favorites"
                        checkable: false
                        onClicked: {
                            if (checked) {
                                root.profile.removeGoal(goal)
                            } else {
                                root.profile.addGoal(goal)
                            }
                        }
                    }
                    PlasmaComponents.Label {
                        anchors.verticalCenter: parent.verticalCenter
                        height: paintedHeight
                        font.pointSize: theme.defaultFont.pointSize
                        text: model.title
                    }
                }

                PlasmaComponents.ScrollBar {
                    flickableItem: languageList
                }
            }

            Row {
                spacing: 10
                anchors.horizontalCenter: parent.horizontalCenter
                width: childrenRect.width
                height: childrenRect.height
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
            }
        }
    }

    Item {
        id: editorContainer
        width: parent.width - 40
        height: childrenRect.height
        anchors.centerIn: parent
        ProfileForm {
            id: profileForm
            width: parent.width
            height: childrenRect.height
            showWelcomeLabel: false
            onDone: {
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

    Item {
        id: deleteConfirmationContainer
        width: parent.width - 40
        height: childrenRect.height
        anchors.centerIn: parent
        Column {
            width: parent.width
            height: childrenRect.height
            spacing: 15

            PlasmaComponents.Label {
                property string name
                id: deleteConfirmationLabel
                width: parent.width
                text: i18n("Do you really want to delete the profile \"<b>%1</b>\"?", name)
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
            }
            Row {
                spacing: 10
                anchors.horizontalCenter: parent.horizontalCenter
                width: childrenRect.width
                height: childrenRect.height
                PlasmaComponents.ToolButton {
                    iconSource: "edit-delete"
                    text: i18n("Delete")
                    onClicked: root.deletionRequest()
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
            name: "init"
            PropertyChanges {
                target: infoContainer
                visible: false
            }
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
            name: "info"
            PropertyChanges {
                target: infoContainer
                visible: true
            }
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
                target: infoContainer
                visible: false
            }
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
                target: infoContainer
                visible: false
            }
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
