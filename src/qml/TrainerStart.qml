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

    Row {
        id: languageControls
        anchors { top: header.bottom; left: screen.left; topMargin: 30; leftMargin: 30 }
        spacing: 10

        Text {
            anchors { verticalCenter : languageSwitcher.verticalCenter }
            text : i18n("Language:")
            font.bold : true
            font.pointSize : theme.defaultFont.pointSize
        }

        LanguageSwitcher {
            id: languageSwitcher
            width: Math.max(400, Math.floor(screen.width/3))
            resourceManager: globalResourceManager
            view: kcfg_UseContributorResources ? LanguageModel.NonEmptyLanguages : LanguageModel.NonEmptyGhnsOnlyLanguages
            onLanguageSelected: {
                screen.languageSelected(selectedLanguage)
            }
        }
    }

    PlasmaComponents.ToolButton {
        id: knsDownloadButton
        anchors { top: languageControls.top; right: parent.right; rightMargin: 50 }
        text: i18n("Download new courses")
        iconSource: "get-hot-new-stuff"
        height: 48
        flat: false
        enabled: true
        onClicked : {
            downloadNewStuff()
        }
    }

    Item {
        id: breadcrumb

        anchors { top: languageControls.bottom; left: languageControls.left; topMargin: 20 }
        width: parent. width
        height: 150

    }

    Text {
        id: selectNextTipp

        anchors { top: breadcrumb.bottom; left: languageControls.left; topMargin: 40 }
        text: {
            if (userProfile.language != null && userProfile.course == null) {
                return i18n("Select a course:");
            }
            if (userProfile.course != null && userProfile.unit == null) {
                if (!screen.__showPhonemeUnits) {
                    return i18n("Select a scenario unit:");
                }
                else {
                    return i18n("Select a phoneme unit:");
                }
            }
            return i18n("Training is starting...");
        }
        font.pointSize: 24;
    }

    Column {
        spacing: 30
        anchors { top: selectNextTipp.top; left: selectNextTipp.right; leftMargin: 30; topMargin: 10 }

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

        Column {
            visible: userProfile.course != null && userProfile.unit == null && screen.__showPhonemeUnits == false
            UnitSelector {
                id: unitSelector
                width: Math.max(300, screen.width - selectNextTipp.width - 80)
                height: screen.height - breadcrumb.height - 230
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
                course: userProfile.course
                onUnitSelected: {
                    userProfile.unit = unit //TODO remove after porting to training session
                    trainingSession.createFromUnit(unit);
                    screen.unitSelected(unit)
                }
            }
        }
    }

    // additional information for current selector
    PlasmaComponents.ButtonColumn {
        anchors { top: selectNextTipp.bottom; left: selectNextTipp.left; topMargin: 20; leftMargin: 20 }
        visible: userProfile.course != null

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
