// SPDX-FileCopyrightText: 2015-2025 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.artikulate

pragma ComponentBehavior: Bound

Kirigami.Page {
    id: root
    title: i18nc("@title:page", "Repository and course selection")

    titleDelegate: RowLayout {
        Label {
            Layout.preferredWidth: width
            text: i18n("Repository:")
        }
        TextField {
            readOnly: true
            text: g_repository.repositoryUrl.toString().replace(/^(file:\/{2})/,"");
        }
        Button {
            Layout.preferredWidth: width
            text: i18n("Change Folder")
            icon.name: "document-open-folder"
            onClicked: repositorySelectionDialog.open()
            FolderDialog {
                id: repositorySelectionDialog
                modality: Qt.WindowModal
                title: i18n("Select Repository Folder")
                currentFolder: g_repository.repositoryUrl
                onAccepted: {
                    console.log("Accepted: " + repositorySelectionDialog.selectedFolder)
                    g_repository.repositoryUrl = repositorySelectionDialog.selectedFolder
                }
            }
        }
        Item {
             Layout.fillWidth: true
        }
    }

    ScrollView {
        anchors.fill: parent
        clip: true
        ScrollBar.horizontal.interactive: false
        ScrollBar.vertical.interactive: true

        Kirigami.CardsListView {
            width: root.width - 40
            model: CourseModel { }

            delegate: Kirigami.AbstractCard {
                id: item
                required property string description
                required property string title
                required property Language language
                required property var dataRole

                contentItem: Item {
                    implicitHeight: delegateLayout.implicitHeight
                    GridLayout {
                        id: delegateLayout
                        anchors {
                            left: parent.left
                            top: parent.top
                            right: parent.right
                        }
                        rowSpacing: Kirigami.Units.largeSpacing
                        columnSpacing: Kirigami.Units.largeSpacing
                        columns: width > Kirigami.Units.gridUnit * 20 ? 4 : 2
                        Kirigami.Icon {
                            source: "artikulate-language"
                            Layout.fillHeight: true
                            Layout.maximumHeight: Kirigami.Units.iconSizes.huge
                            Layout.preferredWidth: height
                        }
                        ColumnLayout {
                            Kirigami.Heading {
                                level: 2
                                text: i18nc("@title:window language / course name", "%1 / %2", item.language.title, item.title)
                            }
                            Kirigami.Separator {
                                Layout.fillWidth: true
                            }
                            Label {
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                                text: item.description
                            }
                        }
                        Button {
                            Layout.alignment: Qt.AlignRight|Qt.AlignVCenter
                            Layout.columnSpan: 2
                            text: i18nc("@action:button", "Edit Course")
                            onClicked: {
                                showPassiveNotification("Selected course for editor: " + item.title + ".");
                                EditorSession.course = item.dataRole
                            }
                        }
                    }
                }
            }
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }

        Kirigami.CardsListView {
            width: root.width - 40
            model: SkeletonModel {}

            delegate: Kirigami.AbstractCard {
                id: skeletonItem
                required property string description
                required property string title
                required property var dataRole

                contentItem: Item {
                    implicitHeight: skeletonDelegateLayout.implicitHeight
                    GridLayout {
                        id: skeletonDelegateLayout
                        anchors {
                            left: parent.left
                            top: parent.top
                            right: parent.right
                        }
                        rowSpacing: Kirigami.Units.largeSpacing
                        columnSpacing: Kirigami.Units.largeSpacing
                        columns: width > Kirigami.Units.gridUnit * 20 ? 4 : 2
                        Kirigami.Icon {
                            source: "artikulate-language"
                            Layout.fillHeight: true
                            Layout.maximumHeight: Kirigami.Units.iconSizes.huge
                            Layout.preferredWidth: height
                        }
                        ColumnLayout {
                            Kirigami.Heading {
                                level: 2
                                text: i18nc("@title:window prototype name", "%1", skeletonItem.title)
                            }
                            Kirigami.Separator {
                                Layout.fillWidth: true
                            }
                            Label {
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                                text: skeletonItem.description
                            }
                        }
                        Button {
                            Layout.alignment: Qt.AlignRight|Qt.AlignVCenter
                            Layout.columnSpan: 2
                            text: i18nc("@action:button", "Edit Prototype")
                            onClicked: {
                                showPassiveNotification("Selected prototype for editor: " + skeletonItem.title + ".");
                                EditorSession.course = skeletonItem.dataRole
                            }
                        }
                    }
                }
            }
        }
    }
}
