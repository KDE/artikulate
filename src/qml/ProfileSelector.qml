/*
 *  Copyright 2012       Sebastian Gottfried <sebastiangottfried@web.de>
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
            if (profileManager.activeProfile == profileManager.profile(i)) {
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
                                if (profileManager.activeProfile == profileManager.profile(index)) {
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
