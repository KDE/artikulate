/*
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

Item {
    id: root

    property Language selectedLanguage
    property ResourceManager resourceManager
    property ProfileManager manager: profileManager

    signal languageSelected(variant language)

    width: 300
    height: Math.max(buttonLeft.height, languageView.height)

    Connections {
        target: profileManager
        onActiveProfileChanged: {
            if (languageView.count == 0) {
                root.selectedLanguage = null
                languageSelected(null)
            } else {
                // TODO remember language by learner profile
                // currently, this triggers change of index
                languageView.decrementCurrentIndex()
            }
        }
    }

    Component {
        id: itemDelegate

        Row {
            id: languageInfo
            property LearningGoal learningGoal: model.dataRole
            property Language language
            onLearningGoalChanged: {
                languageInfo.language = resourceManager.language(learningGoal)
            }
            width: root.width - buttonLeft.width - buttonRight.width - 20
            height: theme.mediumIconSize
            spacing: 10
            Connections {
                target: languageView
                onCurrentIndexChanged: {
                    selectedLanguage = language
                    languageSelected(language)
                }
            }
            PlasmaCore.IconItem {
                id: icon
                source: "artikulate-language"
                width: theme.mediumIconSize
                height: theme.mediumIconSize
                anchors.verticalCenter: parent.verticalCenter
            }
            PlasmaComponents.Label {
                id: languageTitleLabel
                anchors.verticalCenter: parent.verticalCenter
                height: paintedHeight
                font.pointSize: 1.5 * theme.defaultFont.pointSize
                text: language != null ? language.title + " / " + language.i18nTitle : ""
            }
        }
    }

    ListView {
        id: languageView

        width: root.width - buttonLeft.width - buttonRight.width - 20
        height: theme.mediumIconSize

        clip: true
        snapMode: ListView.SnapToItem
        orientation: ListView.Vertical
        model: LearningGoalModel {
            id: learningGoalModel
            profileManager: root.manager
            learner: root.manager.activeProfile
        }
        delegate: itemDelegate
    }

    Row {
        visible: languageView.count == 0
        spacing: 10
        anchors {
            left: languageView.left
            top: languageView.top
        }
        PlasmaCore.IconItem {
            id: icon
            source: "dialog-information"
            width: theme.mediumIconSize
            height: theme.mediumIconSize
            anchors.verticalCenter: parent.verticalCenter
        }
        PlasmaComponents.Label {
            id: favoritesUnsetInformation
            anchors.verticalCenter: parent.verticalCenter
            height: paintedHeight
            font.pointSize: 1.5 * theme.defaultFont.pointSize
            text: i18n("Please select a favorite language")
        }
    }

    PlasmaComponents.ToolButton {
        id: buttonLeft
        anchors {
            left: languageView.right
            leftMargin: 10
            top: languageView.top
        }
        iconSource: "arrow-left"
        enabled: languageView.currentIndex > 0 && languageView.count > 0
        onClicked: {
            languageView.decrementCurrentIndex()
        }
    }

    PlasmaComponents.ToolButton {
        id : buttonRight
        anchors {
            left: buttonLeft.right
            leftMargin: 10
            top: languageView.top
        }
        enabled: languageView.currentIndex <  languageView.count - 1 && languageView.count > 0
        iconSource: "arrow-right"
        onClicked: {
            languageView.incrementCurrentIndex()
        }
    }
}
