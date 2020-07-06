/*
 *  Copyright 2018  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.5
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.7 as Kirigami
import artikulate 1.0

Kirigami.GlobalDrawer {
    id: root

    title: "Artikulate"
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
                action: Kirigami.Action {
                    text: i18n("Training")
                    iconName: "artikulate"
                    onTriggered: {
                        root.pageStack.clear();
                        root.pageStack.push(welcomePageComponent);
                    }
                }
            }
            Kirigami.Separator {
                Layout.fillWidth: true
            }
        }
    ]

    // ordinary Kirigami actions are filled from training units/phrases
    actions: sessionActions.actions
    DrawerTrainingActions {
        id: sessionActions
        session: g_trainingSession
        onTriggerPhraseView: {
            root.pageStack.clear();
            root.pageStack.push(trainingPageComponent);
        }
    }
    Connections {
        target: g_trainingSession
        onCloseUnit: {
            root.resetMenu()
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

        Kirigami.Separator {
            Layout.fillWidth: true
        }

//TODO currently disabled while contents have to be ported
//        ActionListItem {
//            action: Kirigami.Action {
//                text: i18n("Statistics")
//                iconName: "user-properties"
//                onTriggered: {
//                    root.pageStack.pop();
//                    root.pageStack.push(profileSettingsPageComponent);
//                }
//            }
//        }
//        ActionListItem {
//            action: Kirigami.Action {
//                text: i18n("Settings")
//                iconName: "settings-configure"
//                onTriggered: triggerSettingsDialog()
//            }
//        }
        ActionListItem {
            action: Kirigami.Action {
                text: i18n("Download Training")
                iconName: "get-hot-new-stuff"
                onTriggered: {
                    root.pageStack.pop();
                    root.pageStack.push(downloadPageComponent);
                }
            }
        }
        ActionListItem {
            action: Kirigami.Action {
                text: i18n("About Artikulate")
                iconName: "help-about"
                onTriggered: {
                    if (root.pageStack.layers.depth < 2) {
                        root.pageStack.layers.push(aboutPageComponent)
                    }
                }
            }
        }
    }
}
