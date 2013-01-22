/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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

    property LanguageModel languageModel
    property CourseModel courseModel
    property UnitModel unitModel
    signal languageSelected(variant language)
    signal courseSelected(variant course)
    signal unitSelected(variant unit)

    QtObject {
        id: d
    }

    function start() {}
    function reset() {
        //TODO
    }

    Column {
        anchors.fill: parent
        spacing: 30

        PlasmaComponents.ToolBar {
            id: header
            width: parent.width
            tools: Row {
                anchors.leftMargin: 3
                anchors.rightMargin: 3
                spacing: 5

                PlasmaComponents.ToolButton {
                    id: configureButton
                    iconSource: "configure"
                    onClicked: {
                        var position = mapToItem(null, 0, height)
                        showMenu(position.x, position.y)
                    }
                }
            }
        }

        Text {
            text: "<h1>ArtiKulate</h1>"
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Row {
            spacing: 20
            anchors.horizontalCenter: parent.horizontalCenter

            Column {
                Text { text: "<h2>Languages</b>" }
                LanguageSelector {
                    id: languageSelector
                    languageModel: screen.languageModel
                    onLanguageSelected: {
                        screen.languageSelected(language)
                    }
                }
            }

            Column {
                Text { text: "<h2>Courses</h2>" }
                CourseSelector {
                    id: courseSelector
                    onCourseSelected: {
                        screen.courseSelected(course)
                    }
                }
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                text: "<h2>Units</h2>"
            }
            UnitSelector {
                id: unitSelector
                unitModel: screen.unitModel
                onUnitSelected: {
                    screen.unitSelected(unit)
                }
            }
        }
    }
}
