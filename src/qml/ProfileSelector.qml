/*
 *  SPDX-FileCopyrightText: 2012 Sebastian Gottfried <sebastiangottfried@web.de>
 *  SPDX-FileCopyrightText: 2013-2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.1
import QtQuick.Controls 1.2
import artikulate 1.0

FocusScope {
    id: root

    signal profileChosen(variant profile)

    function createNewProfile() {
        var profile = profileManager.addProfile(i18n("Unnamed Identity"))
        profileForm.profile = profile
    }

    function selectProfile(index) {
        list.currentIndex = index
        profileForm.profile = profileManager.profile(index)
    }

    Component.onCompleted : {
        profileForm.profile = profileManager.activeProfile
        list.currentIndex = -1 // clear highlighting as fallback
        for (var i=0; i < profileManager.profileCount; ++i) {
            if (profileManager.activeProfile === profileManager.profile(i)) {
                list.currentIndex = i
                return;
            }
        }
    }

    Column {
        anchors.fill: parent
        spacing: 10

        Row {
            height: parent.height - selectButton.height - parent.spacing
            width: parent.width
            spacing: 10

            ScrollView {
                id: listContainer
                height: parent.height
                width: Math.round((parent.width - parent.spacing) / 2)

                ListView {
                    id: list
                    anchors.fill: parent
                    model: profileManager.profileCount + 1
                    clip: true
                    delegate: ListItem {
                        property bool isNewButton: index >= profileManager.profileCount
                        // update list-index when profilemanager is changed
                        Connections {
                            target: profileManager
                            onActiveProfileChanged: {
                                if (profileManager.activeProfile === profileManager.profile(index)) {
                                    list.currentIndex = index
                                }
                            }
                        }
                        width: list.width - 10
                        title: isNewButton?
                                   i18n("Create New Learner Identity"):
                                   index < profileManager.profileCount? profileManager.profile(index).name: null
                        label.font.italic: isNewButton
                        iconName: isNewButton? "list-add": "user-identity"
                        onSelected: {
                            list.currentIndex = index
                            if (isNewButton) {
                                createNewProfile()
                            }
                            else {
                                selectProfile(index)
                            }
                        }
                    }
                }
            }

            ProfileDetailsItem {
                id: profileForm
                width: parent.width - listContainer.width - parent.spacing
                height: parent.height

                onDeletionRequest: {
                    profileManager.removeProfile(profileForm.profile)
                    profileForm.profile = null
                }
            }
        }

        Button {
            id: selectButton
            anchors.horizontalCenter: parent.horizontalCenter
            iconName: "go-next-view"
            text: i18n("Use Selected Identity")
            enabled: list.currentIndex !== -1 && list.currentIndex < profileManager.profileCount
            onClicked: {
                root.profileChosen(profileManager.profile(list.currentIndex))
                profileManager.activeProfile = profileManager.profile(list.currentIndex)
            }
        }
    }
}
