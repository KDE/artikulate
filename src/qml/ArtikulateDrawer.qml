/*
 *  Copyright 2018  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.0 as Kirigami

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

    actions: [
        Kirigami.Action {
            text: i18n("Welcome")
            iconName: "artikulate"
            onTriggered: {
                root.pageStack.clear();
                root.pageStack.push(welcomePageComponent);
            }
        },
        Kirigami.Action {
            text: i18n("Training")
            iconName: "document-open"
            onTriggered: {
                root.pageStack.clear();
                root.pageStack.push(trainingItemsPageComponent);
                root.pageStack.push(trainingPageComponent);
            }
        },
        Kirigami.Action {
            text: i18n("Help")
            iconName: "help-about"
            Kirigami.Action {
                text: i18n("Artikulate Handbook")
                iconName: "help-contents"
                onTriggered: {
                    triggerAction("help_contents");
                    globalDrawer.resetMenu();
                }
            }
            Kirigami.Action {
                text: i18n("Report Bug")
                iconName: "tools-report-bug"
                onTriggered: {
                    triggerAction("help_report_bug");
                    globalDrawer.resetMenu();
                }
            }
            Kirigami.Action {
                text: i18n("About Artikulate")
                iconName: "artikulate"
                onTriggered: {
                    triggerAction("help_about_app")
                    globalDrawer.resetMenu();
                }
            }
            Kirigami.Action {
                text: i18n("About KDE")
                iconName: "help-about"
                onTriggered: {
                    triggerAction("help_about_kde")
                    globalDrawer.resetMenu();
                }
            }
        }
    ]

    ColumnLayout {
        spacing: 0
        Layout.fillWidth: true
        Layout.leftMargin: -root.leftPadding
        Layout.rightMargin: -root.rightPadding

        Kirigami.Separator {
            Layout.fillWidth: true
        }

        ActionListItem {
            action: Kirigami.Action {
                text: i18n("Statistics")
                iconName: "user-properties"
                onTriggered: {
                    root.pageStack.pop();
                    root.pageStack.push(profileSettingsPageComponent);
                }
            }
        }
        ActionListItem {
            action: Kirigami.Action {
                text: i18n("Settings")
                iconName: "settings-configure"
                onTriggered: triggerSettingsDialog()
            }
        }
        ActionListItem {
            action: Kirigami.Action {
                text: i18n("Download Training")
                iconName: "get-hot-new-stuff"
                onTriggered: triggerDownloadCourses()
            }
        }
    }
}
