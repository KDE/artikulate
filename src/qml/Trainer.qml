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

import artikulate 1.0
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    id: trainer

    property ResourceManager resourceManager: globalResourceManager

    height: parent.height

    function switchScreen(from, to) {
        switchScreenAnimation.from = from
        switchScreenAnimation.to = to
        switchScreenAnimation.start()
    }

    ApplicationBackground {
        id: background
        anchors.fill: parent
    }

    CourseModel {
        id: availableCourseModel
        resourceManager: globalResourceManager
    }

    // course close button
    PlasmaComponents.ToolButton {
        id: closeButton
        anchors { top: trainer.top; right: trainer.right; rightMargin: 30; topMargin: 30}
        visible: trainingScreen.visible
        iconSource: "go-up"
        height: 48
        text: i18n("Close Course")
        onClicked: {
            // show overview screen / start screen
            userProfile.course = null
            switchScreen(trainingScreen, overviewScreen)
        }
    }

    Column {
        width: trainer.width
        anchors { top: trainer.top }

        TrainerOverviewScreen {
            id: overviewScreen
            width: parent.width
            height: trainer.height - 30
            visible: false
            focus: true

            onLanguageSelected: {
                availableCourseModel.language = language
                userProfile.language = language
            }

            onUnitSelected: {
                switchScreen(overviewScreen, trainingScreen)
            }

            Component.onCompleted: {
                overviewScreen.reset()
                overviewScreen.visible = true
            }
        }

        TrainerCourse {
            id: trainingScreen
            width: parent.width
            height: trainer.height - 30
            visible: false
            session: trainingSession //TODO we do not need global object for this
            onCloseCourse: {
                userProfile.course = null
                switchScreen(trainingScreen, overviewScreen)
            }
            onCloseUnit: {
                userProfile.unit = null
                switchScreen(trainingScreen, overviewScreen)
            }
        }
    }

    Rectangle {
        id: curtain
        anchors.fill: parent
        color: "#000"
        opacity: 0
    }

    SequentialAnimation
    {
        id: switchScreenAnimation
        property Item from
        property Item to
        NumberAnimation {
            target: curtain
            property: "opacity"
            to: 1
            duration: switchScreenAnimation.to == overviewScreen ? 250 : 750
            easing.type: Easing.OutQuad
        }
        PropertyAction {
            target: switchScreenAnimation.from
            property: "visible"
            value: false
        }
        PropertyAction {
            target: switchScreenAnimation.to
            property: "visible"
            value: true
        }
        NumberAnimation {
            target: curtain
            property: "opacity"
            to: 0
            duration: switchScreenAnimation.to == overviewScreen ? 250 : 750
            easing.type: Easing.InQuad
        }
        ScriptAction {
            script: {
                switchScreenAnimation.to.forceActiveFocus()
            }
        }
    }
}
