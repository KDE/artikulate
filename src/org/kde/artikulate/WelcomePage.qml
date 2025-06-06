// SPDX-FileCopyrightText: 2015-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-FileCopyrightText: 2023 Carl Schwan <carl@carlschwan.eu>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.newstuff as KNS
import org.kde.artikulate

Kirigami.ScrollablePage {
    id: root
    title: i18n("Welcome to Artikulate")

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
                        source: "languages"
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
                        text: i18nc("@action:button", "Start Training")
                        onClicked: {
                            showPassiveNotification("Starting training session for course " + model.title + ".");
                            TrainingSession.course = model.dataRole
                            TrainingSession.trainingStarted()
                        }
                    }
                }
            }
        }

        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent
            visible: listView.count === 0
            width: parent.width - Kirigami.Units.gridUnit * 4
            text: i18n("No trainings found")
            helpfulAction: KNS.Action {
                configFile: ":/artikulate/config/artikulate.knsrc"
                viewMode: KNS.Page.ViewMode.Preview
                text: i18n("Download Training")
                onEntryEvent: function(entry, event) {
                    if (event === KNS.Entry.StatusChangedEvent) {
                        applicationWindow().ghnsCourseDataStatusChanged();
                    }
                }
            }
        }
    }
}
