// SPDX-FileCopyrightText: 2018 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.artikulate

Kirigami.OverlayDrawer {
    id: root
    bottomPadding: 0
    property QtObject pageStack

    // enforce drawer always to be open
    modal: false
    handleVisible: false

    DrawerCourseTreeModel {
        id: drawerCourseTreeModel
        course: EditorSession.course
        session: EditorSession
        trainingFilter: false
    }

    DrawerTrainingActions {
        id: sessionActions
        model: drawerCourseTreeModel
        onTriggerPhraseView: {
            root.pageStack.clear();
            root.pageStack.push(editCoursePageComponent);
        }
        onCurrentIndexChanged: (index) => {
            phraseActionListView.currentIndex = index
        }
    }

    contentItem: Item {
        Column {
            Column {
                id: topContent

                ActionListItem {
                    width: root.contentItem.width
                    action: QQC2.Action {
                        text: i18n("Courses")
                        icon.name: "artikulate"
                        onTriggered: {
                            root.pageStack.clear();
                            root.pageStack.push(editorCourseSelectionPage);
                            root.pageStack.push(editorSkeletonSelectionPage);
                        }
                    }
                }
                ActionListItem {
                    width: root.contentItem.width
                    action: QQC2.Action {
                        text: i18n("Repository")
                        icon.name: "folder-sync"
                        onTriggered: {
                            root.pageStack.clear();
                            root.pageStack.push(repositoryPageComponent);
                        }
                    }
                }
                Kirigami.Separator {
                    width: root.contentItem.width
                }
                ActionListItem {
                    width: root.contentItem.width
                    action: QQC2.Action {
                        text: i18n("Course Configuration")
                        icon.name: "document-properties"
                        enabled: EditorSession.course !== null
                        onTriggered: {
                            root.pageStack.clear();
                            root.pageStack.push(courseConfigurationPageComponent);
                        }
                    }
                }
            }

            QQC2.ScrollView {
                id: courseActionScrollView
                width: parent.width
                height: root.height - topContent.height - bottomContent.height
                ListView {
                    id: phraseActionListView
                    model: sessionActions
                    //highlightRangeMode: ListView.ApplyRange
                    highlightFollowsCurrentItem: true
                    delegate: ActionListItem {
                        id: phraseAction
                        required property string actionText
                        required property int kDescendantLevel
                        required property bool kDescendantExpanded
                        required property var drawerAction
                        required property int index
                        width: courseActionScrollView.availableWidth
                        text: actionText
                        leftPadding: kDescendantLevel * 20
                        highlighted: phraseAction.index === phraseActionListView.currentIndex
                        action: QQC2.Action {
                            checkable: false
                            onTriggered: {
                                sessionActions.trigger(phraseAction.index)
                                phraseActionListView.currentIndex = phraseAction.index
                            }
                        }
                    }
                }
            }

            Column {
                id: bottomContent
                Kirigami.Separator {
                    width: root.contentItem.width
                }
                ActionListItem {
                    width: root.contentItem.width
                    action: QQC2.Action {
                        text: i18n("Save")
                        icon.name: "document-save"
                        enabled: EditorSession.course !== null
                        onTriggered: {
                            EditorSession.course.sync()
                        }
                    }
                }
                ActionListItem {
                    width: root.contentItem.width
                    action: Kirigami.Action {
                        text: i18n("About Artikulate")
                        icon.name: "help-about"
                        onTriggered: {
                            if (root.pageStack.layers.depth < 2) {
                                root.pageStack.layers.push(aboutPageComponent)
                            }
                        }
                    }
                }
            }
        }
    }
}
