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
    id: editor

    property Language currentLanguage
    property Course currentCourse
    property Unit currentUnit
    property Course currentSkeleton
    property string currentLanguageName: i18n("Unselected")
    property string currentCourseName: i18n("Unselected")
    property string currentUnitName: i18n("Unselected")

    signal closeEditor()

    width: 400 //parent.width
    height: 400 //parent.height

    onCurrentCourseChanged: {
        currentUnit = null
        if (editor.currentCourse == null) {
            editor.currentCourseName = i18n("Unselected");
        }
        else {
            editor.currentCourseName = currentCourse.title;
        }
    }

    onCurrentLanguageChanged: {
        currentUnit = null
        currentCourse = null
        if (editor.currentLanguage == null) {
            currentLanguageName = i18n("Unselected");
        }
        else {
            editor.currentLanguageName = currentLanguage.title;
        }
    }

    onCurrentUnitChanged: {
        if (editor.currentUnit == null) {
            currentUnitName = i18n("Unselected");
        }
        else {
            editor.currentUnitName = currentUnit.title;
        }
    }

    ApplicationBackground {
        id: background
        anchors.fill: parent
    }

    UnitModel {
        id: selectedUnitModel
        course: editor.currentCourse
    }

    PlasmaComponents.ToolBar {
        id: breadcrumb
        anchors { top: editor.top; left: editor.left}
        width: editor.width
        height: 30

        tools: Row {
            width: parent.width
            anchors { leftMargin: 3; rightMargin: 3 }
            spacing: 5

            Row {
                id: selectorInformation
                width: 300
                height: parent.height
                anchors.verticalCenter: parent.verticalCenter

                Row {
                    visible: !editCourseSelector.isSkeleton
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: i18n("<strong>Language:</strong> %1", editor.currentLanguageName)
                    }
                    PlasmaComponents.ToolButton { // unselect-button for language
                        id: unselectLanguage
                        anchors.verticalCenter: parent.verticalCenter
                        iconSource: "dialog-close"
                        flat: true
                        enabled: editor.currentLanguage != null
                        onClicked: {
                            editCourseSelector.unselect()
                        }
                    }
                    Text {
                        anchors { verticalCenter: parent.verticalCenter }
                        text: i18n("<strong>Course:</strong> %1", editor.currentCourseName)
                    }
                    PlasmaComponents.ToolButton { // sync button for skeleton
                        anchors.verticalCenter: parent.verticalCenter
                        text: i18n("Update from Course Prototype")
                        iconSource: "svn-update"
                        flat: true
                        enabled: { editor.currentCourse != null }
                        onClicked: {
                            globalResourceManager.updateCourseFromSkeleton(editor.currentCourse);
                        }
                    }
                }
                Row {
                    visible: editCourseSelector.isSkeleton
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: i18n("<strong>Skeleton:</strong> %1", editor.currentCourseName)
                    }
                    PlasmaComponents.ToolButton { // unselect-button for language
                        anchors.verticalCenter: parent.verticalCenter
                        iconSource: "dialog-close"
                        flat: true
                        enabled: editor.currentCourse != null
                        onClicked: {
                            editCourseSelector.unselect()
                        }
                    }
                }
            }
            Row {
                width: breadcrumb.width - selectorInformation.width - 10
                layoutDirection: Qt.RightToLeft
                Row {
                    PlasmaComponents.ToolButton {
                        anchors.verticalCenter: parent.verticalCenter
                        text: i18n("OK")
                        visible: {
                            if (currentCourse != null) return currentCourse.modified;
                            else return false;
                        }
                        enabled: {
                            if (currentCourse != null) return currentCourse.modified;
                            else return false;
                        }
                        iconSource: "dialog-ok-apply"
                        onClicked: {
                            editor.currentCourse.sync();
                            editCourseSelector.unselect()
                        }
                    }
                    PlasmaComponents.ToolButton {
                        anchors.verticalCenter: parent.verticalCenter
                        text: i18n("Cancel")
                        visible: {
                            if (currentCourse != null) return currentCourse.modified;
                            else return false;
                        }
                        enabled: {
                            if (currentCourse != null) return currentCourse.modified;
                            else return false;
                        }
                        iconSource: "dialog-cancel"
                        onClicked: {
                            globalResourceManager.reloadCourseOrSkeleton(editor.currentCourse)
                            editCourseSelector.unselect()
                        }
                    }
                    PlasmaComponents.ToolButton {
                        anchors.verticalCenter: parent.verticalCenter
                        text: i18n("Close Course")
                        visible: {
                            if (currentCourse != null) return !currentCourse.modified;
                            else return false;
                        }
                        iconSource: "dialog-close"
                        onClicked: {
                            editCourseSelector.unselect()
                        }
                    }
                }
                Row {
                    PlasmaComponents.ToolButton { // unselect-button for language
                        anchors.verticalCenter: parent.verticalCenter
                        iconSource: "go-up"
                        text: i18n("Close Editor")
                        visible: {
                            if (currentCourse != null) return false;
                            else return true;
                        }
                        flat: true
                        onClicked: {
                            editCourseSelector.unselect()
                            closeEditor()
                        }
                    }
                }
            }
        }
    }

    Column {
        id: main

        anchors { top: breadcrumb.bottom; left: editor.left; topMargin: 30; leftMargin: 30 }
        width: editor.width - 60
        height: editor.height - breadcrumb.height - 60
        spacing: 20

        Text {
            id: editorTitle
            text: i18n("Course Editor")
            font.pointSize: 28;
        }
        EditorSelector {
            id: editCourseSelector
            anchors { horizontalCenter: parent.horizontalCenter }
            onSelectedLanguageChanged: {
                editor.currentLanguage = selectedLanguage
            }
            onSelectedCourseChanged: {
                editor.currentLanguage = selectedLanguage
                editor.currentCourse = selectedCourse
            }
        }

        Row {
            visible: currentCourse != null
            width: parent.width
            height: parent.height - editorTitle.height - 50

            Column {
                id: unitSelectorColumn
                width: 200

                Text {
                    id: unitSelectorCaption
                    text: i18n("Units")
                    font.pointSize: 20;
                }
                PlasmaComponents.ToolButton {
                    id: unitAddButton
                    text: i18n("Add Unit")
                    iconSource: "document-new"
                    enabled: editor.currentCourse != null
                    onClicked: {
                        editor.currentCourse.createUnit()
                    }
                }
                UnitSelector {
                    id: unitSelector
                    height: main.height - unitSelectorCaption.height - unitAddButton.height - editorTitle.height
                    unitModel: selectedUnitModel
                    onUnitSelected: {
                        editor.currentUnit = unit;
                    }
                }
            }

            UnitEditor {
                width: main.width - unitSelectorColumn.width
                height: parent.height
                unit: editor.currentUnit
            }
        }
    }
}
