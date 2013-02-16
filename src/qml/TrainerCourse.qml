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

    property Course course
    property Unit selectedUnit
    property string unitName

    onCourseChanged: {
        screen.selectedUnit = null
    }

    UnitModel {
        id: selectedUnitModel
        course: screen.course
    }

    PhraseModel {
        id: currentPhrasesModel
        unit: screen.selectedUnit
    }

    unitName: {
        if (selectedUnit) {
            selectedUnit.title
        } else {
            "unselected"
        }
    }

    Row {
        height: 50

        Item { // spacer
            width: 10
            height: parent.height
        }

        Text {
            text: {
                if (course == null) {
                    ""
                } else {
                    "<h1>" + i18n("Course: %1", course.title) + "</h1>";
                }
            }
        }
    }

    Row {
        y: 50
        Column {
            width: 200

            Text {
                text: i18n("<h2>Units</h2>")
            }

            UnitSelector {
                id: unitSelector
                unitModel: selectedUnitModel
                onUnitSelected: {
                    selectedUnit = unit
                }
            }
        }

        Column {
            Text {
                text: "<strong>" + i18n("Current Unit") + "</strong> " + unitName
            }

            TrainingUnit {
                phraseModel: currentPhrasesModel
            }
        }

//         Column {
//             Row {
//                 PlasmaComponents.ToolButton {
//                     iconSource: "go-previous-view"
//                     text: i18n("Previous Unit")
//                     onClicked: {
//                         //TODO
//                     }
//                 }
//                 PlasmaComponents.ToolButton {
//                     iconSource: "go-next-view"
//                     text: i18n("Next Unit")
//                     onClicked: {
//                         //TODO
//                     }
//                 }
//             }
//
//
//         }
    }
}
