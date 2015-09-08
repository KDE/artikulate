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
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import org.kde.kquickcontrolsaddons 2.0
import artikulate 1.0

Item
{
    id: root

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
            id: languageRow
            Label {
                text: i18n("Language")
            }
            ComboBox {
                Layout.minimumWidth: 200
                Layout.fillWidth: true
                enabled: !buttonEditSkeleton.checked
                model: LanguageModel {
                    id: languageModel
                    view: LanguageModel.AllLanguages
                    resourceModel: LanguageResourceModel {
                        resourceManager: g_resourceManager
                    }
                }
                textRole: "i18nTitle"
                onCurrentIndexChanged: {
                    editorSession.language = languageModel.language(currentIndex)
                }
            }
            Button {
                id: buttonEditSkeleton
                text: i18n("Edit Skeleton")
                iconName: "code-class"
                checkable: true
            }
        }
        RowLayout {
            id: courseRow
            Label {
                text: i18n("Course")
            }
            ComboBox {
                id: comboCourse
                visible: !buttonEditSkeleton.checked
                Layout.minimumWidth: 200
                Layout.fillWidth: true
                model: CourseModel {
                    id: courseModel
                    resourceManager: g_resourceManager
                    language: editorSession.language
                    onLanguageChanged: {
                        if (courseModel.course(0)) {
                            editorSession.course = courseModel.course(0)
                        }
                    }
                }
                textRole: "title"
                onCurrentIndexChanged: {
                    if (courseModel.course(currentIndex)) {
                        editorSession.course = courseModel.course(currentIndex)
                    }
                }
                onVisibleChanged: {
                    if (visible && courseModel.course(currentIndex)) {
                        editorSession.course = courseModel.course(currentIndex)
                    }
                }
            }
            ComboBox {
                id: comboSkeleton
                visible: buttonEditSkeleton.checked
                Layout.minimumWidth: 200
                Layout.fillWidth: true
                model: SkeletonModel {
                    id: skeletonModel
                    resourceManager: g_resourceManager
                }
                textRole: "title"
                onCurrentIndexChanged: {
                    if (skeletonModel.course(currentIndex)) {
                        editorSession.course = skeletonModel.course(currentIndex)
                    }
                }
                onVisibleChanged: {
                    if (visible && skeletonModel.course(currentIndex)) {
                        editorSession.course = skeletonModel.course(currentIndex)
                    }
                }
            }
        }
        RowLayout {
            id: mainRow
            height: main.height - languageRow.height - courseRow.height - 2 * 15
            ScrollView {
                Layout.minimumWidth: Math.floor(main.width * 0.3)
                Layout.fillHeight: true
                TreeView {
                    height: mainRow.height
                    width: Math.floor(main.width * 0.3) - 20
                    TableViewColumn {
                        title: i18n("Units & Phrases")
                        role: "text"
                    }
                    model: PhraseModel {
                        id: phraseModel
                        course: editorSession.course
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
                        editorSession.phrase = phraseModel.phrase(index)
                    }
                }
            }
            PhraseEditor {
                phrase: editorSession.phrase
                isSkeletonPhrase: buttonEditSkeleton.checked
                Layout.minimumWidth: Math.floor(main.width * 0.6)
                Layout.fillHeight: true
            }
        }
    }
}
