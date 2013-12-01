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

    signal deletionRequest();

    Locale.Locale {
        id: locale
    }

    Item {
        id: infoContainer
        width: root.width
        height: root.height
        anchors.centerIn: parent
        visible: profile != null

        PlasmaComponents.TabBar {
            id: tabbar
            width: parent.width
            anchors { top: parent.top }

            PlasmaComponents.TabButton {
                text: i18n("User")
                tab: userPage
            }
            PlasmaComponents.TabButton {
                text: i18n("Favorite Languages")
                tab: favoriteLanguages
            }
        }

        PlasmaComponents.TabGroup {
            anchors { top: tabbar.bottom; left: tabbar.left; right: tabbar.right; }
            ProfileDetailsViewUser {
                id: userPage
                profile: root.profile
                onDeletionRequest: {
                    root.deletionRequest()
                }
            }

            PlasmaComponents.Page {
                id: favoriteLanguages
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
            }
        }
    }
}
