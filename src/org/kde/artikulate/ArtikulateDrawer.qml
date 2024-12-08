// SPDX-FileCopyrightText: 2018-2024 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.newstuff as KNS
import org.kde.artikulate

Kirigami.OverlayDrawer {
    id: root

    bottomPadding: 0
    property Kirigami.PageRow pageStack
    property QtObject parentWindow

    // enforce drawer always to be open
    modal: false
    handleVisible: false

    // ordinary Kirigami actions are filled from training units/phrases
    DrawerTrainingActions {
        id: sessionActions
        session: TrainingSession
        onTriggerPhraseView: {
            root.pageStack.clear();
            root.pageStack.push(trainingPageComponent);
        }
    }
    Connections {
        target: TrainingSession
        function onCloseUnit() {
            root.resetMenu()
        }
    }

    contentItem: QQC2.ScrollView {
        width: parent.width
        height: parent.height
        ColumnLayout {
            anchors.fill: parent
            spacing: 0
            Layout.leftMargin: -root.leftPadding
            Layout.rightMargin: -root.rightPadding
            ActionListItem {
                action: QQC2.Action {
                    text: i18n("Training")
                    icon.name: "artikulate"
                    onTriggered: {
                        root.pageStack.clear();
                        root.pageStack.push(welcomePageComponent);
                    }
                }
            }
            Kirigami.Separator {
                Layout.fillWidth: true
            }

            Repeater {
                id: unit
                model: sessionActions.actions
                delegate: Column {
                    id: menu
                    required property TrainingAction modelData
                    readonly property string text: modelData.text
                    readonly property string iconName: modelData.icon.name
                    property var subActions: modelData.children
                    property bool checked: menuEntry.checked
                    width: parent.width
                    ActionListItem {
                        width: parent.width
                        action: QQC2.Action {
                            id: menuEntry
                            checkable: true
                            text: menu.text
                            icon.name: menu.iconName
                            onTriggered: {
                                menu.modelData.trigger()
                            }
                        }
                    }
                    Column {
                        id: submenu
                        width: parent.width
                        QQC2.ButtonGroup {
                            buttons: submenu.children
                        }
                        Repeater {
                            id: phrasemenu
                            model: menu.checked ? menu.subActions : undefined
                            delegate: ActionListItem {
                                id: phraseAction
                                required property TrainingAction modelData
                                width: parent.width
                                leftPadding: 20
                                text: phraseAction.modelData.text
                                action: QQC2.Action {
                                    checkable: true
                                    icon.name: phraseAction.modelData.icon.name
                                    onTriggered: {
                                        phraseAction.modelData.trigger()
                                    }
                                }
                                Connections {
                                    target: phraseAction.modelData
                                    function onCheckedChanged(checked) {
                                        phraseAction.checked = checked
                                    }
                                }
                            }
                        }
                    }
                }
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
//                 icon.name: "help-about"
//                 onTriggered: {
//                     triggerAction("help_about_kde")
//                     globalDrawer.resetMenu();
//                 }
//             }
//         }
//     ]

    // ColumnLayout {
    //     spacing: 0
    //     Layout.fillWidth: true
    //     Layout.leftMargin: -root.leftPadding
    //     Layout.rightMargin: -root.rightPadding



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
    }

    Column {
        anchors.bottom: parent.bottom
        width: parent.width
        Kirigami.Separator {
            Layout.fillWidth: true
        }
        ActionListItem {
            width: parent.width
            action: KNS.Action {
                text: i18n("Download Training")
                configFile: ":/artikulate/config/artikulate.knsrc"
                transientParent: root.parentWindow
                viewMode: KNS.Page.ViewMode.Preview
                onEntryEvent: function(entry, event) {
                    if (event === KNS.Entry.StatusChangedEvent) {
                        applicationWindow().ghnsCourseDataStatusChanged();
                    }
                }
            }
        }
        ActionListItem {
            width: parent.width
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
