/*
 *  Copyright 2013-2017  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

import QtQuick 2.5
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.0 as Kirigami2
import artikulate 1.0

Kirigami2.Page {
    id: root

    title: {
        var titleString = "";
        if (g_trainingSession.unit === null) {
            titleString += i18n("Category: no category selected");
        }
        else {
            titleString += i18n("Category: ") + g_trainingSession.unit.title
        }
        if (g_trainingSession.unit !== null && g_trainingSession.course !== null) {
            titleString += " / " + g_trainingSession.course.i18nTitle
        }
        return titleString
    }
    actions {
        main: Kirigami2.Action {
            text: i18n("Next")
            iconName: "dialog-ok"
            onTriggered: g_trainingSession.showNextPhrase()
        }
        right: Kirigami2.Action {
            text: i18n("Skip")
            iconName: "go-next"
            enabled: g_trainingSession.hasNextPhrase
            onTriggered: g_trainingSession.skipPhrase()
        }
    }

    Item {
        width: root.width - (root.leftPadding + root.rightPadding);
        height: root.height - root.topPadding;

        Rectangle {
            id: trainingTextRect
            width: parent.width/2 - 20 - 20
            height: phraseText.height + buttonNativePlay.height + 20
            anchors {
                left: parent.left
                top: parent.top
                leftMargin: 20
                topMargin: 20
            }
            color: "lightsteelblue"

            QQC2.TextArea {
                id: phraseText
                anchors {
                    fill: trainingTextRect
                    bottomMargin: buttonNativePlay.height + 20
                }
                objectName: "phraseText"
                text: (g_trainingSession.phrase != null) ? g_trainingSession.phrase.text : ""
                wrapMode: Text.WordWrap
                readOnly: true
                horizontalAlignment: Text.AlignHCenter
            }
            SoundPlayer {
                id: buttonNativePlay
                anchors {
                    bottom: trainingTextRect.bottom
                    horizontalCenter: parent.horizontalCenter
                }
                text: i18n("Listen")
                fileUrl: g_trainingSession.phrase == null ? "" : g_trainingSession.phrase.soundFileUrl
            }
        }

        Rectangle {
            id: trainingUserRect
            width: parent.width/2 - 20 - 20
            height: profilePicture.height + rowSoundControls.height + 20
            anchors {
                right: parent.right
                top: parent.top
                rightMargin: 20
                topMargin: 20
            }
            color: "lightsteelblue"

            ProfileUserImageItem {
                id: profilePicture
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    top: parent.top
                    bottom: parent.bottom
                    bottomMargin: rowSoundControls.height + 20
                }
                width: height
                profile: g_profileManager.activeProfile
            }
            Row {
                id: rowSoundControls
                anchors {
                    bottom: trainingUserRect.bottom
                    horizontalCenter: trainingUserRect.horizontalCenter
                    leftMargin: 20
                    bottomMargin: 20
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
}
