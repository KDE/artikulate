/*
 *  Copyright 2015-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import QtQuick.Controls 2.1 as QQC2
import QtQuick.Layouts 1.3
import org.kde.kirigami 2.7 as Kirigami
import artikulate 1.0

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
                implicitWidth: delegateLayout.implicitWidth
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
                        text: i18nc("@action:button", "Select Course")
                        onClicked: {
                            showPassiveNotification("Selected course for editor: " + model.title + ".");
                            g_editorSession.course = model.dataRole
                        }
                    }
                }
            }
        }
    }
}
