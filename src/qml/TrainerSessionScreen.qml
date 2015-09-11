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

    ColumnLayout {
        id: main
        anchors.fill: parent

        Column {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            Text {
                font.pointSize: 24
                text: {
                    if (g_trainingSession.unit == null) {
                        return i18n("Category: no category selected");
                    }
                    return i18n("Category: ") + g_trainingSession.unit.title
                }
            }
            Text {
                text: {
                    if (g_trainingSession.unit == null) {
                        return "";
                    }
                    return g_trainingSession.course.i18nTitle
                }
            }
        }

        RowLayout {
            Layout.alignment: Qt.AlignCenter
            Layout.fillWidth: true
            spacing: 30
            Rectangle {
                id: trainingTextRect
                Layout.alignment: Qt.AlignTop
                Layout.fillWidth: true
                Layout.preferredHeight: main.height / 2
                color: "lightsteelblue"

                Text {
                    id: phraseText
                    anchors {
                        fill: trainingTextRect
                        leftMargin: 30
                        topMargin: 30
                        rightMargin: 30
                        bottomMargin: 30
                    }
                    objectName: "phraseText"
                    text: (g_trainingSession.phrase != null) ? g_trainingSession.phrase.text : ""
                    wrapMode: Text.WordWrap
                }
                SoundPlayer {
                    id: buttonNativePlay
                    anchors {
                        bottom: trainingTextRect.bottom
                        leftMargin: 30
                        bottomMargin: 30
                    }
                    text: i18n("Listen")
                    Layout.alignment: Qt.AlignBottom
                    fileUrl: g_trainingSession.phrase == null ? "" : g_trainingSession.phrase.soundFileUrl
                }
            }
            Rectangle {
                id: trainingUserRect
                Layout.alignment: Qt.AlignTop
                Layout.preferredWidth: recorder.width + player.width + 30
                Layout.preferredHeight: main.height / 2
                color: "lightsteelblue"

                QIconItem {
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        verticalCenter: parent.verticalCenter
                    }
                    width: 200
                    height: 200
                    icon: "user-identity"
                }
                Row {
                    anchors {
                        bottom: trainingUserRect.bottom
                        horizontalCenter: trainingUserRect.horizontalCenter
                        leftMargin: 30
                        bottomMargin: 30
                    }
                    SoundRecorder {
                        id: recorder
                    }
                    SoundPlayer {
                        id: player
                        fileUrl: recorder.outputFileUrl
                    }
                }
            }
        }
        RowLayout {
            Layout.alignment: Qt.AlignRight
            spacing: 20

            FlatButton {
                text: i18n("Skip")
                iconName: "go-next"
                iconSize: 48
                fontSize: 20
                enabled: g_trainingSession.hasNextPhrase
                onClicked: {
                    g_trainingSession.skipPhrase()
                }
            }

            FlatButton {
                text: i18n("Next")
                iconName: "dialog-ok"
                iconSize: 48
                fontSize: 20
                onClicked: {
                    g_trainingSession.showNextPhrase()
                }
            }
        }
    }
}
