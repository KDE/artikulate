/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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

Item
{
    id: main

    property ResourceManager resourceManager: globalResourceManager
    property Unit selectedUnit

    function switchScreen(from, to) {
        switchScreenAnimation.from = from
        switchScreenAnimation.to = to
        switchScreenAnimation.start()
    }

    ApplicationBackground {
        id: background
        anchors.fill: parent
    }

    LanguageModel {
        id: availableLanguageModel
        resourceManager: globalResourceManager
    }

    CourseModel {
        id: availableCourseModel
        resourceManager: globalResourceManager
    }

    UnitModel {
        id: availableUnitModel
    }

    PhraseModel {
        id: availablePhraseModel
    }

    HomeScreen {
        id: homeScreen
        anchors.fill: parent
        visible: false
        focus: true
        languageModel: availableLanguageModel
        courseModel: availableCourseModel
        unitModel: availableUnitModel

        onLanguageSelected: {
            availableCourseModel.language = language
            availableUnitModel.course = null
        }
        onCourseSelected: {
            availableUnitModel.course = course
        }
        onUnitSelected: {
            availablePhraseModel.unit = unit
            selectedUnit = unit
            switchScreen(homeScreen, trainingScreen)
        }

        Component.onCompleted: {
            homeScreen.reset()
            homeScreen.visible = true
        }
    }

    TrainingScreen {
        id: trainingScreen
        phraseModel: availablePhraseModel
        anchors.fill: parent
        visible: false
        unit: main.selectedUnit
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
            duration: switchScreenAnimation.to == homeScreen? 250: 750
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
            duration: switchScreenAnimation.to == homeScreen? 250: 750
            easing.type: Easing.InQuad
        }
        ScriptAction {
            script: {
                switchScreenAnimation.to.forceActiveFocus()
            }
        }
    }
}
