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
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Layouts 1.2
import QtQml.Models 2.2
import artikulate 1.0

Item {
    id: root

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

    ColumnLayout {
        id: main

        anchors {
            fill: parent
            topMargin: 20
            rightMargin: 20
            bottomMargin: 20
            leftMargin: 20
        }
        spacing: 10

        RowLayout {
            Label {
                text: i18n("Course Prototype:")
            }
            ComboBox {
                Layout.minimumWidth: 300
                model: SkeletonModel {
                    id: skeletonModel
                    repository: g_repository
                }
                textRole: "title"
                onCurrentIndexChanged: {
                    g_editorSession.skeleton = skeletonModel.skeleton(currentIndex)
                }
            }
            Button {
                id: buttonEditSkeleton
                Layout.minimumWidth: 200
                text: i18n("Edit Prototype")
                icon.name: "code-class"
                checkable: true
                enabled: g_editorSession.skeletonMode
                onClicked: g_editorSession.editSkeleton = checked
            }
            Item { Layout.fillWidth: true }
            Button {
                id: buttonSyncFromSkeleton
                enabled: !buttonEditSkeleton.checked
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
            id: languageRow
            Label {
                text: i18n("Language:")
            }
            ComboBox {
                id: languageSelectionComboBox
                Layout.minimumWidth: 200
                Layout.fillWidth: true
                enabled: !buttonEditSkeleton.checked
                model: languageModel
                textRole: "i18nTitle"
                onCurrentIndexChanged: {
                    g_editorSession.setCourseByLanguage(languageModel.language(currentIndex))
                }
            }
        }
        RowLayout {
            id: createNewCourseRow
            visible: {
                if (buttonEditSkeleton.checked || g_editorSession.displayedCourse !== null) {
                    return false
                }
                return true
            }

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
            Item { Layout.fillHeight: true } //dummy
        }
        RowLayout {
            id: mainRow
            visible: g_editorSession.course !== null && createNewCourseRow.visible === false
            Layout.fillHeight: true
            ColumnLayout {
                ScrollView {
                    Layout.minimumWidth: Math.floor(main.width * 0.3)
                    Layout.fillHeight: true
                    QQC1.TreeView {
                        id: phraseTree
                        height: {
                            mainRow.height
                            - (newUnitButton.visible ? newUnitButton.height : 0)
                            - 10
                        }
                        width: Math.floor(main.width * 0.3) - 20
                        QQC1.TableViewColumn {
                            title: i18n("Units & Phrases")
                            role: "text"
                        }
                        model: PhraseModel {
                            id: phraseModel
                            course: g_editorSession.displayedCourse
                        }
                        selection: ItemSelectionModel {
                            model: phraseTree.model
                        }
                        itemDelegate: Item {
                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                color: styleData.textColor
                                elide: styleData.elideMode
                                text: styleData.value
                            }
                        }
                        onClicked: {
                            if (phraseModel.isPhrase(index)) {
                                g_editorSession.phrase = phraseModel.phrase(index)
                            } else {
                                g_editorSession.phrase = null
                                g_editorSession.unit = phraseModel.unit(index)
                            }
                        }
                        Connections {
                            target: g_editorSession
                            onPhraseChanged: {
                                if (g_editorSession.phrase === null) {
                                    return
                                }
                                phraseTree.expand(phraseModel.indexUnit(g_editorSession.phrase.unit))
                                phraseTree.selection.setCurrentIndex(
                                    phraseModel.indexPhrase(g_editorSession.phrase),
                                    ItemSelectionModel.ClearAndSelect)
                            }
                        }
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
                UnitEditor {
                    visible: g_editorSession.unit !== null && g_editorSession.phrase === null
                    unit: g_editorSession.unit
                    editPhrases: g_editorSession.skeletonMode && g_editorSession.editSkeleton
                }
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
}
