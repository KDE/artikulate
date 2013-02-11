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

import QtQuick 1.0
import artikulate 1.0
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents

Item
{
    id: editor
    property Language currentLanguage
    property Course currentCourse
    property Unit currentUnit
    property string currentLanguageName: i18n("Unselected")
    property string currentCourseName: i18n("Unselected")

    onCurrentCourseChanged: {
        currentUnit = null
        if (editor.currentCourse == null) i18n("Unselected")
        else editor.currentCourseName = currentCourse.title;
    }

    onCurrentLanguageChanged: {
        currentUnit = null
        currentCourse = null
        if (editor.currentLanguage == null) i18n("Unselected")
        else editor.currentLanguageName = currentLanguage.title;
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
        language: editor.currentLanguage
        resourceManager: globalResourceManager
    }

    UnitModel {
        id: selectedUnitModel
        course: editor.currentCourse
    }

    Column {
        anchors.fill: parent

        PlasmaComponents.ToolBar {
            id: header
            width: parent.width
            height: 30
            tools: Row {
                anchors.leftMargin: 3
                anchors.rightMargin: 3
                spacing: 5

                Row {
                    width: 200
                    height: parent.height
                    anchors.verticalCenter: parent.verticalCenter
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: i18n("<strong>Language:</strong> %1", editor.currentLanguageName)
                    }
                    PlasmaComponents.ToolButton { // unselect-button for language
                        anchors.verticalCenter: parent.verticalCenter
                        iconSource: "dialog-close"
                        flat: true
                        enabled: { availableCourseModel.language != null }
                        onClicked: {
                            currentCourse = null
                            currentLanguage = null
                        }
                    }
                }
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: i18n("<strong>Course:</strong> %1", editor.currentCourseName)
                    width: 200
                }
                Row {
                    visible: {
                        if (currentCourse != null) return currentCourse.modified;
                        else return false;
                    }
                    PlasmaComponents.ToolButton {
                        anchors.verticalCenter: parent.verticalCenter
                        text: i18n("OK")
                        enabled: {
                            if (currentCourse != null) return currentCourse.modified;
                            else return false;
                        }
                        iconSource: "dialog-ok-apply"
                        onClicked: {
                            //TODO save course
                        }
                    }
                    PlasmaComponents.ToolButton {
                        anchors.verticalCenter: parent.verticalCenter
                        text: i18n("Cancel")
                        enabled: {
                            if (currentCourse != null) return currentCourse.modified;
                            else return false;
                        }
                        iconSource: "dialog-cancel"
                        onClicked: {
                            globalResourceManager.reloadCourse(editor.currentCourse)
                            editor.currentCourse = null
                        }
                    }
                }
                PlasmaComponents.ToolButton {
                    anchors.verticalCenter: parent.verticalCenter
                    text: i18n("Close")
                    visible: {
                        if (currentCourse != null) return !currentCourse.modified;
                        else return false;
                    }
                    iconSource: "dialog-close"
                    onClicked: {
                        editor.currentCourse = null
                    }
                }
            }
        }

        Text {
            text: i18n("<h1>Course Editor</h1>")
        }

        Row {
            spacing: 20

            Column {
                visible: { currentLanguage == null }
                Text {
                    text: i18n("<h2>Select Language</h2>")
                }
                LanguageSelector {
                    id: languageSelector
                    languageModel: availableLanguageModel
                    onLanguageSelected: {
                        editor.currentLanguage = language
                   }
                }
            }

            Column {
                visible: {
                    if (currentLanguage == null) return false;
                    if (currentCourse != null) return false
                    return true
                }
                Text { text: i18n("<h2>Select Course</h2>") }
                PlasmaComponents.ToolButton {
                    text: i18n("New Course")
                    iconSource: "document-new"
                    enabled: {
                        availableCourseModel.language != null
                    }
                    onClicked: {
                        globalResourceManager.newCourseDialog();
                    }
                }
                CourseSelector {
                    id: courseSelector
                    courseModel: availableCourseModel
                    onCourseSelected: {
                        editor.currentCourse = course
                    }
                }
            }
        }
        Row {
            visible: { currentCourse != null }
            Column {
                width: 200

                Text {
                    text: i18n("<h2>Units</h2>")
                }

                UnitSelector {
                    id: unitSelector
                    unitModel: selectedUnitModel
                    onUnitSelected: {
                        editor.currentUnit = unit;
                    }
                }
            }

            Column {
//                 Text {
//                     text: "<strong>" + i18n("Current Unit") + "</strong> " + unitName
//                 }

                UnitEditor {
                    unit: editor.currentUnit
                }
            }
        }
    }
}
