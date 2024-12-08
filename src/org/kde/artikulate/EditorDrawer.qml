// SPDX-FileCopyrightText: 2018 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.artikulate

Kirigami.GlobalDrawer {
    id: root

    title: "Editor"
    titleIcon: "artikulate"
    resetMenuOnTriggered: false
    bottomPadding: 0
    property QtObject pageStack

    // enforce drawer always to be open
    modal: false
    handleVisible: false

    topContent: [
        ColumnLayout {
            spacing: 0
            Layout.fillWidth: true
            Layout.leftMargin: -root.leftPadding
            Layout.rightMargin: -root.rightPadding
            ActionListItem {
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
                Layout.fillWidth: true
            }
            ActionListItem {
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
    ]

    // ordinary Kirigami actions are filled from training units/phrases
    actions: trainingActions.actions
    DrawerTrainingActions {
        id: trainingActions
        session: EditorSession
        onTriggerPhraseView: {
            root.pageStack.clear();
            root.pageStack.push(editCoursePageComponent);
        }
    }

//TODO integrate again
//     [
//         Kirigami.Action {
//             text: i18n("Help")
//             iconName: "help-about"
//             Kirigami.Action {
//                 text: i18n("Artikulate Handbook")
//                 iconName: "help-contents"
//                 onTriggered: {
//                     triggerAction("help_contents");
//                     globalDrawer.resetMenu();
//                 }
//             }
//             Kirigami.Action {
//                 text: i18n("Report Bug")
//                 iconName: "tools-report-bug"
//                 onTriggered: {
//                     triggerAction("help_report_bug");
//                     globalDrawer.resetMenu();
//                 }
//             }
//             Kirigami.Action {
//                 text: i18n("About KDE")
//                 iconName: "help-about"
//                 onTriggered: {
//                     triggerAction("help_about_kde")
//                     globalDrawer.resetMenu();
//                 }
//             }
//         }
//     ]

    ColumnLayout {
        spacing: 0
        Layout.fillWidth: true
        Layout.leftMargin: -root.leftPadding
        Layout.rightMargin: -root.rightPadding

        ActionListItem {
            action: QQC2.Action {
                text: i18n("Save")
                icon.name: "document-save"
                enabled: EditorSession.course !== null
                onTriggered: {
                    EditorSession.course.sync()
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }

//TODO planned but not implemented
//        ActionListItem {
//            action: Kirigami.Action {
//                text: i18n("Upload Training")
//                iconName: "get-hot-new-stuff"
//                onTriggered: {
//                    root.pageStack.pop();
//                    root.pageStack.push(downloadPageComponent);
//                }
//            }
//        }
        ActionListItem {
            action: QQC2.Action {
                text: i18n("About Artikulate Editor")
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
