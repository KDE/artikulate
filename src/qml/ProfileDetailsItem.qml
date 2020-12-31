/*
    SPDX-FileCopyrightText: 2012 Sebastian Gottfried <sebastiangottfried@web.de>
    SPDX-FileCopyrightText: 2013-2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import artikulate 1.0

Item {
    id: root

    property Learner profile: null
    property ProfileManager manager: profileManager

    signal deletionRequest();
/*
    Locale.Locale {
        id: locale
    }*/

    Item {
        id: infoContainer
        width: root.width
        height: root.height
        anchors.centerIn: parent
        visible: profile != null

        TabView {
            anchors.fill: parent
            anchors.margins: 4

            Tab {
                id: leanerTab
                title: i18n("Learner")
                ProfileDetailsViewUser {
                    id: userPage
                    width: parent.width
                    height: parent.height
                    profile: root.profile
                    onDeletionRequest: {
                        root.deletionRequest()
                    }
                }
            }

            Tab {
                title: i18n("Favorite Languages")
                ProfileDetailsViewFavorites {
                    id: favoriteLanguages
                    width: parent.width
                    height: parent.height
                    profile: root.profile
                    profileManager: root.manager
                }
            }

            style: TabViewStyle {
                frameOverlap: 1
                tab: Rectangle {
                    id: tabRect
                    color: styleData.selected ? "#c0e7f9" : "#ffffff"
                    border.width: 0
                    implicitWidth: Math.max(text.width + 8, 80)
                    implicitHeight: 24
                    Text {
                        id: text
                        anchors.centerIn: parent
                        text: styleData.title
                        color: "black"
                    }
                }
                frame: Rectangle {
                    color: "#c0e7f9"
                }
            }
        }
    }
}
