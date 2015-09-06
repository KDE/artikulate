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

Item
{
    id: editor

    property Language currentLanguage: editorSession.language
    property Course currentCourse: editorSession.course
    property Unit currentUnit: editorSession.unit
    property Course currentSkeleton
    property string currentLanguageName: i18n("Unselected")
    property string currentCourseName: i18n("Unselected")
    property string currentUnitName: i18n("Unselected")

    signal closeEditor()

    width: 400 //parent.width
    height: 400 //parent.height

    Item {
        id: theme
        property string backgroundColor: "#ffffff"
        property int smallIconSize: 18
        property int smallMediumIconSize: 22
        property int mediumIconSize: 32
        property int fontPointSize: 11
    }

    onCurrentCourseChanged: {
        editorSession.unit = null
        if (editorSession.course == null) {
            editor.currentCourseName = i18n("Unselected");
        }
        else {
            editor.currentCourseName = editorSession.course.title;
        }
    }

    onCurrentLanguageChanged: {
        editorSession.unit = null
        editorSession.course = null
        if (editorSession.language == null) {
            currentLanguageName = i18n("Unselected");
        }
        else {
            editor.currentLanguageName = editorSession.language.title;
        }
    }

    onCurrentUnitChanged: {
        if (editorSession.unit == null) {
            editor.currentUnitName = i18n("Unselected");
        }
        else {
            editor.currentUnitName = editorSession.unit.title;
        }
    }

    UnitModel {
        id: selectedUnitModel
        course: editorSession.course
    }

    ToolBar {
        id: header
        anchors { top: editor.top; left: editor.left}
        width: editor.width

        Item {
            width: parent.width
            height: buttonUpdateFromPrototype.height
            anchors {
                leftMargin: 3
                rightMargin: 3
            }

            ToolButton { // sync button for skeleton
                id: buttonUpdateFromPrototype
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                }
                text: i18n("Update from Course Prototype")
                iconName: "svn-update"
                enabled: { editorSession.course != null }
                onClicked: {
                    globalResourceManager.updateCourseFromSkeleton(editorSession.course);
                }
            }

            ToolButton { // unselect-button for language
                id: buttonCloseEditor
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                }
                iconName: "go-up"
                text: i18n("Close Editor")
                visible: {
                    if (editorSession.course != null) return false;
                    else return true;
                }
                onClicked: {
                    editCourseSelector.unselect()
                    closeEditor()
                }
            }
            // course edit control buttons
            Row {
                anchors {
                    right: parent.right
                }
                ToolButton {
                    anchors.verticalCenter: parent.verticalCenter
                    text: i18n("OK")
                    visible: {
                        if (editorSession.course != null) return editorSession.course.modified;
                        else return false;
                    }
                    enabled: {
                        if (editorSession.course != null) return editorSession.course.modified;
                        else return false;
                    }
                    iconName: "dialog-ok-apply"
                    onClicked: {
                        editorSession.course.sync();
                        editCourseSelector.unselect()
                    }
                }
                ToolButton {
                    anchors.verticalCenter: parent.verticalCenter
                    text: i18n("Cancel")
                    visible: {
                        if (editorSession.course != null) return editorSession.course.modified;
                        else return false;
                    }
                    enabled: {
                        if (editorSession.course != null) return editorSession.course.modified;
                        else return false;
                    }
                    iconName: "dialog-cancel"
                    onClicked: {
                        globalResourceManager.reloadCourseOrSkeleton(editorSession.course)
                        editCourseSelector.unselect()
                    }
                }
                ToolButton {
                    anchors.verticalCenter: parent.verticalCenter
                    text: i18n("Close Course")
                    visible: {
                        if (editorSession.course != null) return !editorSession.course.modified;
                        else return false;
                    }
                    iconName: "go-up"
                    onClicked: {
                        editCourseSelector.unselect()
                    }
                }
            }
        }
    }

    Rectangle {
        id: breadcrumb
        width: parent.width
        height: languageIcon.height + 6
        anchors {
            top: header.bottom
            topMargin: 2
        }
        color: theme.backgroundColor

        Row {
            id: selectorInformation
            height: 30

            spacing: 20
            anchors {
                topMargin: 3
                top: parent.top
                bottomMargin: 3
                bottom: parent.bottom
                leftMargin: 5
                left: parent.left
            }

            Row {
                visible: !editCourseSelector.isSkeleton
                spacing: 10
                QIconItem {
                    id: languageIcon
                    icon: "artikulate-language"
                    width: theme.mediumIconSize
                    height: theme.mediumIconSize
                    anchors.verticalCenter: parent.verticalCenter
                }
                ToolButton { // unselect-button for language
                    id: unselectLanguage
                    anchors.verticalCenter: parent.verticalCenter
                    iconName: "go-up"
                    enabled: editorSession.language != null
                    onClicked: {
                        editCourseSelector.unselect()
                    }
                }
                Label {
                    visible: editorSession.language != null
                    anchors.verticalCenter: parent.verticalCenter
                    text: editor.currentLanguageName
                    font.pointSize: 1.5 * theme.fontPointSize
                }

                Item { //dummy
                    height: parent.height
                    width: 20
                }

                QIconItem {
                    id: courseIcon
                    visible: editorSession.language != null && editorSession.course != null
                    icon: "artikulate-course"
                    width: theme.mediumIconSize
                    height: theme.mediumIconSize
                    anchors.verticalCenter: parent.verticalCenter
                }
                Label {
                    anchors {
                        verticalCenter: parent.verticalCenter
                    }
                    visible: editorSession.language != null && editorSession.course != null
                    text: editor.currentCourseName
                    font.pointSize: 1.5 * theme.fontPointSize
                }
            }
            Row {
                visible: editCourseSelector.isSkeleton
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: i18n("<strong>Skeleton:</strong> %1", editor.currentCourseName)
                    font.pointSize: 1.5 * theme.fontPointSize
                }
                ToolButton { // unselect-button for language
                    anchors.verticalCenter: parent.verticalCenter
                    iconName: "go-up"
                    width: theme.mediumIconSize
                    height: theme.mediumIconSize
                    enabled: editorSession.course != null
                    onClicked: {
                        editCourseSelector.unselect()
                    }
                }
            }
        }
    }

    Column {
        id: main

        anchors {
            top: breadcrumb.bottom
            left: editor.left
            topMargin: 30
            leftMargin: 30
        }
        width: editor.width - 60
        height: editor.height - breadcrumb.height - 80
        spacing: 10

        visible: globalResourceManager.isRepositoryManager

        EditorSelector {
            id: editCourseSelector
            width: editor.width - 60
            height: main.height - unitSelectorCaption.height - 20
            onSelectedLanguageChanged: {
                editorSession.language = selectedLanguage
            }
            onSelectedCourseChanged: {
                editorSession.language = selectedLanguage
                editorSession.course = selectedCourse
            }
        }

        Item {
            visible: editorSession.course != null

            Column {
                id: unitSelectorColumn
                width: 200
                visible: (editorSession.unit == null)

                Text {
                    id: unitSelectorCaption
                    text: i18n("Units")
                    font.pointSize: 1.5 * theme.fontPointSize
                }
                ToolButton {
                    id: unitAddButton
                    text: i18n("Add Unit")
                    iconName: "document-new"
                    enabled: editorSession.course != null
                    onClicked: {
                        editorSession.course.createUnit()
                    }
                }
                UnitSelector {
                    id: unitSelector
                    height: main.height - unitSelectorCaption.height - unitAddButton.height
                    unitModel: selectedUnitModel
                    onUnitSelected: {
                        editorSession.unit = unit;
                    }
                }
            }

            Column {
                visible: (editorSession.unit != null)

                UnitEditor {
                    width: main.width
                    height: main.height - 80
                    unit: editorSession.unit
                    onCloseUnit: {
                        editorSession.unit = null
                    }
                }
            }
        }
    }

    Column {
        anchors {
            top: breadcrumb.bottom
            left: editor.left
            topMargin: 10
            leftMargin: 10
            bottomMargin: 10
        }
        spacing: 10
        visible: !globalResourceManager.isRepositoryManager
        Text {
            text: i18n("No Course Repository Selected")
            font.pointSize: 1.5 * theme.fontPointSize
        }
        Text {
            text: i18n("Editor is only available when Artikulate works on a course data repository. Please see program settings.")
            wrapMode: Text.WordWrap
        }
    }
}
