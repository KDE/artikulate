/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import QtQuick.Layouts 1.2
import org.kde.kquickcontrolsaddons 2.0
import artikulate 1.0

Item
{
    id: root

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
            root.currentCourseName = i18n("Unselected");
        }
        else {
            root.currentCourseName = editorSession.course.title;
        }
    }

    onCurrentLanguageChanged: {
        editorSession.unit = null
        editorSession.course = null
        if (editorSession.language == null) {
            currentLanguageName = i18n("Unselected");
        }
        else {
            root.currentLanguageName = editorSession.language.title;
        }
    }

    onCurrentUnitChanged: {
        if (editorSession.unit == null) {
            root.currentUnitName = i18n("Unselected");
        }
        else {
            root.currentUnitName = editorSession.unit.title;
        }
    }

    UnitModel {
        id: selectedUnitModel
        course: editorSession.course
    }

    Column {
        id: main

        anchors {
            top: root.top
            left: root.left
            topMargin: 30
            leftMargin: 30
        }
        width: root.width - 60
        height: root.height - 80
        spacing: 10

        RowLayout {
            Label {
                text: i18n("Course Repository:")
            }
            Label {
                visible: g_resourceManager.isRepositoryManager
                text: g_resourceManager.repositoryUrl
            }
            Label {
                visible: !g_resourceManager.isRepositoryManager
                text: "no repository set"
                color: "red"
            }
        }

        RowLayout {
            Label {
                text: i18n("Language")
            }
            ComboBox {
                Layout.minimumWidth: 200
                Layout.fillWidth: true
                model: LanguageModel {
                    view: LanguageModel.AllLanguages
                    resourceModel: LanguageResourceModel {
                        resourceManager: g_resourceManager
                    }
                }
                textRole: "i18nTitle"
                onCurrentIndexChanged: {
                    console.log(currentIndex) //FIXME
                }
            }
            ToolButton { //TODO activate
                text: i18n("Edit Skeleton")
            }
        }

//         Item {
//             visible: editorSession.course != null
//
//             Column {
//                 id: unitSelectorColumn
//                 width: 200
//                 visible: (editorSession.unit == null)
//
//                 Text {
//                     id: unitSelectorCaption
//                     text: i18n("Units")
//                     font.pointSize: 1.5 * theme.fontPointSize
//                 }
//                 ToolButton {
//                     id: unitAddButton
//                     text: i18n("Add Unit")
//                     iconName: "document-new"
//                     enabled: editorSession.course != null
//                     onClicked: {
//                         editorSession.course.createUnit()
//                     }
//                 }
//                 UnitSelector {
//                     id: unitSelector
//                     height: main.height - unitSelectorCaption.height - unitAddButton.height
//                     unitModel: selectedUnitModel
//                     onUnitSelected: {
//                         editorSession.unit = unit;
//                     }
//                 }
//             }
//
//             Column {
//                 visible: (editorSession.unit != null)
//
//                 UnitEditor {
//                     width: main.width
//                     height: main.height - 80
//                     unit: editorSession.unit
//                     onCloseUnit: {
//                         editorSession.unit = null
//                     }
//                 }
//             }
//         }
    }
}
