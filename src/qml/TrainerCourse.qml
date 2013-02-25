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

    property alias unit: currentPhrasesModel.unit

    UnitModel {
        id: selectedUnitModel
        course: userProfile.course
    }

    PhraseModel {
        id: currentPhrasesModel
        unit: userProfile.unit
        type: userProfile.phraseType
    }

    Row {
        height: 50

        Item { // spacer
            width: 10
            height: parent.height
        }

        Text {
            text: {
                if (userProfile.course == null) {
                    ""
                } else {
                    "<h1>" + i18n("Course: %1", userProfile.course.title) + "</h1>";
                }
            }
        }
        Item { // spacer
            width: 300
            height: parent.height
        }
        PhraseTypeSelector {
            onTypeSelected: {
                userProfile.phraseType = type
            }
        }
    }

    Row {
        y: 50
        Item { // spacer
            width: 50
            height: parent.height
        }
        Column {
            width: 200

            Text {
                text: i18n("<h2>Units</h2>")
            }

            UnitSelector {
                id: unitSelector
                unitModel: selectedUnitModel
                onUnitSelected: {
                    userProfile.unit = unit
                }
            }
        }

        Column {
            Text {
                text: {
                    var title = i18n("unselected")
                    if (screen.unit != null) {
                        title = screen.unit.title
                    }
                    i18n("<strong>Current Unit:</strong> %1",title)
                }
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
