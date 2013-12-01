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
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import artikulate 1.0

FocusScope {
    id: root

    signal profileChosen(variant profile)

    function createNewProfile() {
        var profile = profileManager.addProfile(i18n("Unnamed User"))
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

            Item {
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
                        width: list.width - scrollBar.width
                        title: isNewButton?
                                   i18n("Create New Profile"):
                                   index < profileManager.profileCount? profileManager.profile(index).name: null
                        label.font.italic: isNewButton
                        iconSource: isNewButton? "list-add": "user-identity"
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

                PlasmaComponents.ScrollBar {
                    id: scrollBar
                    flickableItem: list
                }
            }

            PlasmaCore.SvgItem {
                id: line
                width: naturalSize.width
                height: parent.height
                elementId: "vertical-line"
                svg: PlasmaCore.Svg {
                    imagePath: "widgets/line"
                }
            }

            ProfileDetailsItem {
                id: profileForm
                width: parent.width - listContainer.width - line.width - 2 * parent.spacing
                height: parent.height

                onDeletionRequest: {
                    profileManager.removeProfile(profileForm.profile)
                    profileForm.profile = null
                    state = "init"
                }
            }
        }

        PlasmaComponents.Button {
            id: selectButton
            anchors.horizontalCenter: parent.horizontalCenter
            iconSource: "go-next-view"
            text: i18n("Use Selected Profile")
            enabled: list.currentIndex !== -1 && list.currentIndex < profileManager.profileCount
            onClicked: {
                root.profileChosen(profileManager.profile(list.currentIndex))
                profileManager.activeProfile = profileManager.profile(list.currentIndex)
            }
        }
    }
}
