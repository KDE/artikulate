/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

import QtQuick 2.5
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import QtQml.Models 2.2
import org.kde.kirigami 2.7 as Kirigami
import artikulate 1.0

Kirigami.ScrollablePage {
    id: root
    title: i18n("Edit Course")

    actions {
        left: Kirigami.Action {
            text: i18n("Previous")
            tooltip: i18n("Switch to previous phrase.")
            iconName: "go-previous"
            enabled: g_editorSession.hasPreviousPhrase
            onTriggered: g_editorSession.switchToPreviousPhrase()
        }
        right: Kirigami.Action {
            text: i18n("Next")
            tooltip: i18n("Switch to next phrase.")
            iconName: "go-next"
            enabled: g_editorSession.hasNextPhrase
            onTriggered: g_editorSession.switchToNextPhrase()
        }
    }

    ColumnLayout {
        id: main

        spacing: 10

        LanguageModel {
            id: languageModel
            view: LanguageModel.AllLanguages
            resourceModel: LanguageResourceModel {
                repository: g_repository
            }
        }
        CourseModel {
            id: courseModel
        }
        UnitModel {
            id: selectedUnitModel
            course: g_editorSession.displayedCourse
        }

        RowLayout {
            Item { Layout.fillWidth: true }
            Button {
                id: buttonSyncFromSkeleton
                enabled: g_editorSession.isSkeletonMode
                Layout.minimumWidth: 200
                text: i18n("Sync Prototype")
                icon.name: "view-refresh"
                ToolTip.visible: hovered
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.text: i18n("Update the course with elements from prototype.")
                onClicked: g_editorSession.updateCourseFromSkeleton()
            }
        }

        RowLayout {
            id: createNewCourseRow
            visible: g_editorSession.isSkeletonMode
            Label {
                text: i18n("There is no course in the selected language.")
            }
            ComboBox { // course selection only necessary when we do not edit skeleton derived course
                id: comboCourse
                visible: !g_editorSession.skeletonMode
                Layout.fillWidth: true
                model: CourseFilterModel {
                    id: courseFilterModel
                    courseModel: courseModel
                    language: g_editorSession.language
                }
                textRole: "title"
                onCurrentIndexChanged: {
                    if (courseFilterModel.course(currentIndex)) {
                        g_editorSession.course = courseFilterModel.course(languageSelectionComboBox.currentIndex)
                    }
                }
                onVisibleChanged: {
                    if (visible && courseFilterModel.course(currentIndex)) {
                        g_editorSession.course = courseFilterModel.course(languageSelectionComboBox.currentIndex)
                    }
                }
            }
            Button {
                text: i18n("Create Course")
                icon.name: "journal-new"
                onClicked: {
                    g_editorSession.course = g_repository.createCourse(languageModel.language(languageSelectionComboBox.currentIndex), g_editorSession.skeleton)
                }
            }
            Button { // add units only if skeleton
                id: newUnitButton
                visible: !g_editorSession.skeletonMode || g_editorSession.editSkeleton
                icon.name: "list-add"
                text: i18n("New Unit")
                onClicked: phraseModel.course.createUnit()
            }
        }

        ColumnLayout {
            PhraseEditor {
                visible: g_editorSession.phrase !== null
                phrase: g_editorSession.phrase
                isSkeletonPhrase: g_editorSession.editSkeleton
                Layout.minimumWidth: Math.floor(main.width * 0.6)
                Layout.fillHeight: true
            }
        }
    }
}
