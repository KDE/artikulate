/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *  Copyright 2013  Oindrila Gupta <oindrila.gupta92@gmail.com>
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

import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import artikulate 1.0

FocusScope {
    id: screen

    property TrainingSession session
    property bool finished: session.isFinished

    signal closeCourse()
    signal closeUnit()

    onFinishedChanged: {
        if (finished) {
            trainerMain.visible = false
            finishedInformation.visible = true
            trainingStatusInfo.visible = false
        }
        else {
            trainerMain.visible = true
            finishedInformation.visible = false
            trainingStatusInfo.visible = true
        }
    }

    Row {
        id: unitHeader
        height: 100
        anchors { top: screen.top; left: screen.left; topMargin: 30; leftMargin: 30 }
        spacing: 15

        Column {
            id: breadcrumb
            width: screen.width
            height: 50
            spacing: 10

            Text {
                id: captionCourse
                text: {
                    if (userProfile.course == null) {
                        ""
                    } else {
                        i18n("Course: %1", userProfile.course.title);
                    }
                }
                font.pointSize: 28
            }
            Row {
                Text {
                    anchors { verticalCenter: parent.verticalCenter }
                    text: {
                        var title = i18n("unselected")
                        if (userProfile.unit != null) {
                            title = userProfile.unit.title
                        }
                        i18n("Unit: %1", title)
                    }
                    font.pointSize: 20
                }
            }
        }
    }

    Row {
        id: trainerMain
        width: screen.width - 20
        height: screen.height - 30 - breadcrumb.height - 50 - 50 - trainingStatusInfo.height
        anchors { top: unitHeader.bottom; left: unitHeader.left; topMargin: 50 }

        TrainerSessionScreen {
            width: parent.width
            height: parent.height
            unit: userProfile.unit
            session: screen.session
        }
    }

    Column {
        id: finishedInformation
        anchors { top: unitHeader.bottom; left: unitHeader.left; topMargin: 20; right: screen.right }
        visible: false
        spacing: 10

        Text {
            id: results
            text: i18n("Results")
            font.pointSize: 20
        }

        Row {
            spacing: 15
            anchors { right: parent.right; rightMargin: 30 }

            Rectangle {
                id: rect1
                height: 15
                width: 15
                color: "#2de86c"
            }

            Text {
                id: label1
                text: "Word"
                font.pointSize: 10
            }

            Rectangle {
                id: rect2
                height: 15
                width: 15
                color: "#327bff"
            }

            Text {
                id: label2
                text: "Expression"
                font.pointSize: 10
            }

            Rectangle {
                id: rect3
                height: 15
                width: 15
                color: "#fff13f"
            }

            Text {
                id: label3
                text: "Sentence"
                font.pointSize: 10
            }

            Rectangle {
                id: rect4
                height: 15
                width: 15
                color: "#e85a02"
            }

            Text {
                id: label4
                text: "Paragraph"
                font.pointSize: 10
            }
        }
        TrainerCourseStatistics {
            width: screen.width - 60
            height: screen.height - 30 - breadcrumb.height - 50 - 50
        }
    }

    Column {
        id: trainingStatusInfo
        anchors { top: trainerMain.bottom; left: unitHeader.left }

        spacing: 10
        Text {
            id: trainingText
            text: i18n("Training Progress")
            font.pointSize: theme.defaultFont.pointSize
        }
        TrainerPhraseTypeStatus {
            width: screen.width - 60
            session: screen.session
            sessionType: screen.session.currentType
            onTypeSelected: {
                userProfile.phraseType = type
            }
        }
    }
}
