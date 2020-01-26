/*
 *  Copyright 2015-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.1
import QtQuick.Controls 2.1 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.7 as Kirigami
import artikulate 1.0

Kirigami.ScrollablePage {
    id: root
    title: i18n("Select Prototype")

    Kirigami.CardsListView {
        id: listView
        width: root.width - 40
        model: SkeletonModel {
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
                        source: "language-artikulate"
                        Layout.fillHeight: true
                        Layout.maximumHeight: Kirigami.Units.iconSizes.huge
                        Layout.preferredWidth: height
                    }
                    ColumnLayout {
                        Kirigami.Heading {
                            level: 2
                            text: i18nc("@title:window prototype name", "%1", model.title)
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
                        text: i18nc("@action:button", "Edit Prototype")
                        onClicked: {
                            showPassiveNotification("Selected prototype for editor: " + model.title + ".");
                            g_editorSession.course = model.dataRole
                        }
                    }
                }
            }
        }
    }
}
