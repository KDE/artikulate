// SPDX-FileCopyrightText: 2015-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.artikulate

Kirigami.ScrollablePage {
    id: root
    title: i18n("Welcome to Artikulate Course Editor")

    Kirigami.CardsListView {
        id: listView
        width: root.width - 40
        model: CourseModel {
            id: courseModel
        }

        delegate: Kirigami.AbstractCard {
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
                            text: i18nc("@title:window language / course name", "%1 / %2", model.language.title, model.title)
                        }
                        Kirigami.Separator {
                            Layout.fillWidth: true
                        }
                        QQC2.Label {
                            Layout.fillWidth: true
                            wrapMode: Text.WordWrap
                            text: model.description
                        }
                    }
                    QQC2.Button {
                        Layout.alignment: Qt.AlignRight|Qt.AlignVCenter
                        Layout.columnSpan: 2
                        text: i18nc("@action:button", "Edit Course")
                        onClicked: {
                            showPassiveNotification("Selected course for editor: " + model.title + ".");
                            EditorSession.course = model.dataRole
                        }
                    }
                }
            }
        }
    }
}
