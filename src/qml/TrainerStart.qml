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

FocusScope {
    id: screen

    property bool __showPhonemeUnits: false

    signal languageSelected(variant language)
    signal courseSelected(variant course)
    signal unitSelected(variant unit)

    height: screen.height
    width: screen.width

    QtObject {
        id: d
    }

    UnitModel {
        id: selectedUnitModel
        course: userProfile.course
    }


    function start() {}
    function reset() {
        //TODO
    }

    PlasmaComponents.ToolBar {
        id: header
        width: screen.width
        tools: Row {
            PlasmaComponents.ToolButton {
                text: profileManager.activeProfile == null
                    ? i18n("User Identity")
                    : profileManager.activeProfile.name
                iconSource: "user-identity"
                onClicked: {
                    if (profileSelectorSheet.isOpen()) {
                        profileSelectorSheet.close()
                    }
                    else {
                        profileSelectorSheet.open()
                    }
                }
                checked: profileSelectorSheet.isOpen()
            }
        }
    }

    Rectangle {
        id: languageControls
        width : parent.width
        height :Math.ceil(Math.max(languageSwitcher.height, knsDownloadButton.height) + 6)
        anchors { top : header.bottom; topMargin : 2 }
        color : theme.backgroundColor

        Row {
            spacing : 20
            anchors { topMargin : 3; top: parent.top; bottomMargin : 3; bottom : parent.bottom; leftMargin: 5; left : parent.left }
            LanguageSwitcher {
                id : languageSwitcher
                anchors { verticalCenter : parent.verticalCenter }
                width : screen.width - 6 - knsDownloadButton.width - 20
                resourceManager: globalResourceManager
                view: kcfg_UseContributorResources ? LanguageModel.NonEmptyLanguages : LanguageModel.NonEmptyGhnsOnlyLanguages
                onLanguageSelected: {
                    screen.languageSelected(selectedLanguage)
                }
            }

            PlasmaComponents.ToolButton {
                id: knsDownloadButton
                iconSource: "get-hot-new-stuff"
                anchors { verticalCenter : parent.verticalCenter }
                flat: false
                enabled: true
                onClicked : {
                    downloadNewStuff()
                }
            }
        }
    }

    Column {
        spacing: 30
        anchors { top: languageControls.bottom; left: languageControls.left; leftMargin: 10; topMargin: 10 }

        CourseSwitcher {
            id: courseSelector
            resourceManager: globalResourceManager
            view: kcfg_UseContributorResources ? CourseModel.AllResources : CourseModel.OnlyGetHotNewStuffResources
            language: userProfile.language
            onCourseSelected: {
                userProfile.course = course
                screen.courseSelected(course)
            }
        }


        // additional information for current selector
        PlasmaComponents.ButtonColumn {
            visible: userProfile.course != null
            anchors { leftMargin : 30; left : parent.left }
            PlasmaComponents.RadioButton {
                text: i18n("Scenario Training Units")
                checked: !screen.__showPhonemeUnits
                onClicked : {
                    screen.__showPhonemeUnits = !screen.__showPhonemeUnits
                }
            }
            PlasmaComponents.RadioButton {
                text: i18n("Phoneme Training Units")
                checked: screen.__showPhonemeUnits
                onClicked : {
                    screen.__showPhonemeUnits = !screen.__showPhonemeUnits
                }
            }
        }

        Column {
            visible: userProfile.course != null && userProfile.unit == null && screen.__showPhonemeUnits == false
            UnitSelector {
                id: unitSelector
                anchors { leftMargin : 30; left : parent.left }
                width: Math.floor(300, screen.width/2)
                height: screen.height - 300
                unitModel: selectedUnitModel
                onUnitSelected: {
                    userProfile.unit = unit
                    screen.unitSelected(unit)
                    trainingSession.createFromUnit(unit)
                }
            }
        }

        Column {
            visible: userProfile.course != null && userProfile.unit == null && screen.__showPhonemeUnits == true
            PhonemeUnitSelector {
                id: phonemeUnitSelector
                anchors { leftMargin : 30; left : parent.left }
                course: userProfile.course
                onUnitSelected: {
                    userProfile.unit = unit //TODO remove after porting to training session
                    trainingSession.createFromUnit(unit);
                    screen.unitSelected(unit)
                }
            }
        }

    }

    SheetDialog {
        id: profileSelectorSheet
        anchors { top: screen.top; topMargin: header.height; left: screen.left; bottom: screen.bottom; right: screen.right }
        content: ProfileSelector {
            anchors.fill: parent
            onProfileChosen: {
                profileSelectorSheet.close()
            }
        }
    }
}
