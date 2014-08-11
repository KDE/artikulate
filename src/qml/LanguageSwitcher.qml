/*
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
import org.kde.kquickcontrolsaddons 2.0
import artikulate 1.0

Item {
    id: root

    property Language selectedLanguage
    property ResourceManager resourceManager
    property ProfileManager manager: profileManager

    signal languageSelected(variant language)

    width: 300
    height: Math.max(buttonLeft.height, languageView.height)

    // emit language selection
    Connections {
        target: buttonRight
        onClicked: {
            selectedLanguage = resourceManager.language(learningGoalModel.learningGoal(languageView.currentIndex))
            languageSelected(selectedLanguage)
        }
    }
    Connections {
        target: buttonLeft
        onClicked: {
            selectedLanguage = resourceManager.language(learningGoalModel.learningGoal(languageView.currentIndex))
            languageSelected(selectedLanguage)
        }
    }

    // react on changed goals
    Component.onCompleted: {
        var learner = profileManager.activeProfile;
        if (!learner) {
            return
        }
        var goal = learner.activeGoal(Learner.Language)
        for (var i=0; i < languageView.count; ++i) {
            if (goal.id == learningGoalModel.learningGoal(i).id) {
                languageView.currentIndex = i
            }
        }
    }
    Connections {
        target: learningGoalModel
        onLearnerChanged: {
            var learner = profileManager.activeProfile;
            var goal = learner.activeGoal(Learner.Language)
            for (var i=0; i < languageView.count; ++i) {
                if (goal.id == learningGoalModel.learningGoal(i).id) {
                    languageView.currentIndex = i
                }
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

            QIconItem {
                id: icon
                icon: "artikulate-language"
                width: theme.mediumIconSize
                height: theme.mediumIconSize
                anchors.verticalCenter: parent.verticalCenter
            }
            Label {
                id: languageTitleLabel
                anchors.verticalCenter: parent.verticalCenter
                height: paintedHeight
                font.pointSize: 1.5 * theme.fontPointSize
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
        QIconItem {
            id: icon
            icon: "dialog-information"
            width: theme.mediumIconSize
            height: theme.mediumIconSize
            anchors.verticalCenter: parent.verticalCenter
        }
        Label {
            id: favoritesUnsetInformation
            anchors.verticalCenter: parent.verticalCenter
            height: paintedHeight
            font.pointSize: 1.5 * theme.fontPointSize
            text: i18n("Please select a favorite language")
        }
    }

    ToolButton {
        id: buttonLeft
        anchors {
            left: languageView.right
            leftMargin: 10
            top: languageView.top
        }
        iconName: "arrow-left"
        enabled: languageView.currentIndex > 0 && languageView.count > 0
        onClicked: {
            languageView.decrementCurrentIndex()
        }
    }

    ToolButton {
        id : buttonRight
        anchors {
            left: buttonLeft.right
            leftMargin: 10
            top: languageView.top
        }
        enabled: languageView.currentIndex <  languageView.count - 1 && languageView.count > 0
        iconName: "arrow-right"
        onClicked: {
            languageView.incrementCurrentIndex()
        }
    }
}
