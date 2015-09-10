/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *  Copyright 2013       Magdalena Konkiewicz <konkiewicz.m@gmail.com>
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
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2
import org.kde.kquickcontrolsaddons 2.0
import artikulate 1.0

Item {
    id: root
    width: 500
    height: 500

    ColumnLayout {
        id: main
        anchors {
            fill: parent
            topMargin: 10
            leftMargin: 10
            rightMargin: 10
            bottomMargin: 10 + languageSwitcher.height//FIXME workaround
        }

        Text {
            Layout.fillWidth: true
            Layout.preferredHeight: 30
            font.pointSize: 24
            text: {
                if (g_trainingSession.unit == null) {
                    return i18n("Category: no category selected");
                }
                return i18n("Category: ") + g_trainingSession.unit.title
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            ColumnLayout {
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 200
                    color: "lightsteelblue"
                    Text {
                        id: phraseText
                        anchors {
                            fill: parent
                            leftMargin: 10
                            rightMargin: 10
                            topMargin: 10
                            bottomMargin: 10
                        }
                        objectName: "phraseText"
                        text: (g_trainingSession.phrase != null) ? g_trainingSession.phrase.text : ""
                        wrapMode: Text.WordWrap
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }

                RowLayout {
                    Layout.alignment: Qt.AlignCenter
                    Text {
                        Layout.alignment: Qt.AlignVCenter
                        text: i18n("Listen")
                        font.pointSize: 24
                    }
                    SoundPlayer {
                        id: buttonNativePlay
                        Layout.alignment: Qt.AlignVCenter
                        width: 96
                        height: 96
                        fileUrl: g_trainingSession.phrase == null ? "" : g_trainingSession.phrase.soundFileUrl
                    }
                }

            }
            ColumnLayout {
                Layout.fillHeight: true
                Layout.preferredWidth: 300
                Layout.fillWidth: true
                QIconItem {
                    Layout.alignment: Qt.AlignCenter
                    width: 200
                    height: 200
                    icon: "user-identity"
                }
                RowLayout {
                    Layout.alignment: Qt.AlignCenter
                    Text {
                        Layout.alignment: Qt.AlignVCenter
                        text: i18n("Record")
                        font.pointSize: 24
                    }
                    SoundRecorder {
                        id: recorder
                        Layout.alignment: Qt.AlignVCenter
                        width: 96
                        height: 96
                    }
                    Text {
                        Layout.alignment: Qt.AlignVCenter
                        text: i18n("Play")
                        font.pointSize: 24
                    }
                    SoundPlayer {
                        Layout.alignment: Qt.AlignVCenter
                        width: 96
                        height: 96
                        fileUrl: recorder.outputFileUrl
                    }
                }
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignRight

            Text {
                Layout.alignment: Qt.AlignVCenter
                text: i18n("Skip")
                font.pointSize: 24
                color: g_trainingSession.hasNextPhrase ? "black" : "grey"
            }
            ToolButton {
                height: 96
                width: 96
                QIconItem {
                    anchors.fill: parent
                    icon: "go-next"
                    state: g_trainingSession.hasNextPhrase ? QIconItem.DefaultState : QIconItem.DisabledState//TODO KF5.15 change to "enabled:"
                }
                onClicked: {
                    g_trainingSession.skipPhrase()
                }
            }
            Text { //dummy
                width: 20
            }
            Text {
                Layout.alignment: Qt.AlignVCenter
                text: i18n("Next")
                font.pointSize: 24
            }
            ToolButton {
                height: 96
                width: 96
                QIconItem {
                    anchors.fill: parent
                    icon: "dialog-ok"
                }
                onClicked: {
                    g_trainingSession.showNextPhrase()
                }
            }
        }
    }
}
