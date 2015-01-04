/*
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *  Copyright 2013       Oindrila Gupta <oindrila.gupta92@gmail.com>
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
import org.kde.kquickcontrolsaddons 2.0
import artikulate 1.0

FocusScope {
    id: root

    property TrainingSession session
    property bool finished: session.isFinished

    signal closeCourse()
    signal closeUnit()

    onFinishedChanged: {
        if (finished) {
            trainerMain.visible = false
            finishedInformation.visible = true
            progressBar.visible = false
        }
        else {
            trainerMain.visible = true
            finishedInformation.visible = false
            progressBar.visible = true
        }
    }

    ToolBar {
        id: header
        width: root.width
        // no tools yet
    }

    Rectangle {
        id: trainingControls
        width: parent.width
        height: Math.max(unitIcon.height,stopButton.height) + 6
        anchors {
            top: header.bottom
            topMargin: 2
        }
        color: theme.backgroundColor

        Row {
            spacing: 20
            anchors {
                topMargin: 3
                top: parent.top
                bottomMargin: 3
                bottom: parent.bottom
                leftMargin: 5
                left: parent.left
            }
            QIconItem {
                id: unitIcon
                icon: "artikulate-course"
                width: theme.mediumIconSize
                height: theme.mediumIconSize
                anchors.verticalCenter: parent.verticalCenter
            }

            Label {
                anchors {
                    verticalCenter: parent.verticalCenter
                }
                height: paintedHeight
                width: root.width - 2*20 - unitIcon.width - stopButton.width - 2*5
                text: {
                    var title = i18n("unselected")
                    if (userProfile.unit != null) {
                        title = userProfile.unit.title
                    }
                    i18n("Unit: %1", title)
                }
                font.pointSize: 1.5 * theme.defaultFont.pointSize
            }

            Button {
                id: stopButton
                anchors {
                    verticalCenter: parent.verticalCenter
                }
                iconName: "go-up"
                height: 48
                text: !root.finished ? i18n("Finish Training") : i18n("Close")
                onClicked: {
                    if (!root.finished) {
                        session.stop()
                    } else {
                        closeUnit()
                    }
                }
            }
        }
    }

    TrainerSessionScreen {
        id: trainerMain
        width: root.width - 40
        height: root.height - 30 - trainingControls.height - 50 - 50 - progressBar.height - 30
        anchors {
            top: trainingControls.bottom
            left: parent.left
            topMargin: 50
            leftMargin: 20
        }
        unit: userProfile.unit
        session: root.session
    }

    Column {
        id: finishedInformation
        anchors {
            top: trainingControls.bottom
            left: parent.left
            topMargin: 50
            leftMargin: 30
        }
        visible: false
        spacing: 20

        Label {
            id: results
            text: i18n("Phrases Grouped by Attempts")
            font.pointSize: 1.5 * theme.defaultFont.pointSize
        }
//TODO comment statistics out for now, they have to be integrated into main view
//         TrainerCourseStatistics {
//             width: root.width - 60
//             height: root.height - 30 - trainingControls.height - 50 - 50
//             session: root.session
//         }
    }

    Column {
        id: progressBar
        anchors {
            bottom: root.bottom
            bottomMargin: 20
            left: trainerMain.left
        }

        spacing: 10
        Text {
            id: trainingText
            text: i18n("Training Progress")
            font.pointSize: theme.fontPointSize
        }
        TrainerProgressBar {
            width: trainerMain.width
            session: root.session
            sessionType: root.session.currentType
            onTypeSelected: {
                userProfile.phraseType = type
            }
        }
    }
}
