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

    Row {
        id: unitHeader
        height: 100
        anchors { top: screen.top; left: screen.left; topMargin: 30; leftMargin: 30 }
        spacing: 15

        Column {
            id: breadcrumb
            width: screen.width
            height: 50

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
                    text: {
                        var title = i18n("unselected")
                        if (userProfile.unit != null) {
                            title = userProfile.unit.title
                        }
                        i18n("Unit: %1", title)
                    }
                    font.pointSize: 20
                }
                PlasmaComponents.ToolButton { // unselect-button for language
                    id: unselectUnit
                    iconSource: "dialog-close"
                    onClicked: {
                        unitSelector.visible = true
                    }
                }
            }
        }
    }

    Row {
        anchors { top: unitHeader.bottom; left: unitHeader.left }

        Column {
            id: unitSelector
            width: 200
            visible: false
            Text {
                text: i18n("Change Unit")
                font.pointSize: 20
            }
            UnitSelector {
                width: 100
                height: 200
                unitModel: UnitModel { course: userProfile.course }
                onUnitSelected: {
                    userProfile.unit = unit
                    unitSelector.visible = false
                }
            }
        }

        TrainingUnit {
            width: {
                if (unitSelector.visible) {
                    screen.width - difficultySelector.width - unitSelector.width - 60
                } else {
                    screen.width - difficultySelector.width - 60
                }
            }
            height: screen.height - breadcrumb.height
            unit: userProfile.unit
        }

        Column {
            id: difficultySelector

            spacing: 10
            Text {
                text: i18n("<strong>Niveau</strong>")
            }
            PhraseTypeSelector {
                onTypeSelected: {
                    userProfile.phraseType = type
                }
            }
        }
    }
}
