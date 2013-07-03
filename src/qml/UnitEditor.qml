/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import org.kde.plasma.extras 0.1 as PlasmaExtras
import artikulate 1.0

Item {
    id: root

    property Unit unit
    property bool editMode: false

    signal closeUnit()

    width: 500
    height: 400

    onUnitChanged: {
        unitBreadcrumb.editMode = false
    }

    PhraseModel {
        id: phraseModel
        unit: root.unit
    }

    Component {
        id: itemDelegate

        Row {
            id: phraseRow
            spacing: 5
            anchors.centerIn: parent.center
            height: Math.max(phraseText.height, typeIcon.height) + 10

            property Phrase phrase: model.dataRole

            PlasmaComponents.ToolButton {
                id: enableEdit
                iconSource: "document-properties"
                onClicked: {
                    editMode = true
                    phraseEditor.phrase = phrase
                }
            }

            Image {
                id: typeIcon

                width: 20
                height: 20
                source: {
                    switch (phrase.type) {
                        case Phrase.Word: "../icons/hicolor/64x64/actions/artikulate-word.png"
                            break;
                        case Phrase.Expression: "../icons/hicolor/64x64/actions/artikulate-expression.png"
                            break;
                        case Phrase.Sentence: "../icons/hicolor/64x64/actions/artikulate-sentence.png"
                            break;
                        case Phrase.Paragraph: "../icons/hicolor/64x64/actions/artikulate-paragraph.png"
                            break;
                        default:
                            ""
                    }
                }
            }

            Text {
                id: phraseText
                width: root.width - enableEdit.width - typeIcon.width - 20
                anchors.verticalCenter: enableEdit.verticalCenter
                text: phrase.text
                wrapMode: Text.WordWrap
                color: {
                    if (editCourseSelector.isSkeleton) {
                        return "black";
                    }
                    switch (phrase.editState) {
                    case Phrase.Unknown: "red";
                        break;
                    case Phrase.Translated: "blue";
                        break;
                    case Phrase.Completed: "black";
                        break;
                    default: "purple";
                    }
                }
            }
        }
    }

    Column {
        spacing: 5
        Row {
            id: unitBreadcrumb
            property bool editMode: false
            height: 30
            spacing: 10

            PlasmaComponents.ToolButton {
                anchors.verticalCenter: unitBreadcrumb.verticalCenter
                text: i18n("Unit: ")
                iconSource: "go-up"
                font.pointSize: 20;
                onClicked: {
                    closeUnit()
                }
            }
            Text {
                id: unitTitle
                visible: !unitBreadcrumb.editMode
                anchors.verticalCenter: unitBreadcrumb.verticalCenter
                text: {
                    if (root.unit) {
                        return root.unit.title;
                    } else {
                        return i18n("unselected");
                    }
                }
                font.pointSize: 20;
            }
            PlasmaComponents.TextField {
                id: unitTitleInput
                visible: unitBreadcrumb.editMode
                anchors.verticalCenter: unitBreadcrumb.verticalCenter
                width: Math.max(unitTitle.width + 20, 200)
                text: unitTitle.text
                onAccepted: {
                    root.unit.title = text
                    unitBreadcrumb.editMode = false
                }
            }

            // edit buttons
            PlasmaComponents.ToolButton {
                id: enableUnitEdit
                anchors.verticalCenter: unitBreadcrumb.verticalCenter
                iconSource: "document-properties"
                enabled: root.unit != null
                visible: !unitBreadcrumb.editMode
                onClicked: {
                    unitBreadcrumb.editMode = !unitBreadcrumb.editMode
                }
            }
            PlasmaComponents.ToolButton {
                anchors.verticalCenter: unitBreadcrumb.verticalCenter
                visible: unitBreadcrumb.editMode
                iconSource: "dialog-ok-apply"
                onClicked: {
                    root.unit.title = unitTitleInput.text
                    unitBreadcrumb.editMode = false
                }
            }
            PlasmaComponents.ToolButton {
                anchors.verticalCenter: unitBreadcrumb.verticalCenter
                visible: unitBreadcrumb.editMode
                iconSource: "dialog-cancel"
                onClicked: {
                    unitBreadcrumb.editMode = false
                }
            }
        }

        PlasmaComponents.ToolButton {
            id: addPhraseButton
            anchors.top: unitBreadcrumb.bottom
            text: i18n("Add Phrase")
            iconSource: "document-new"
            enabled: root.unit != null && !root.editMode
            onClicked: {
                root.unit.course.createPhrase(unit)
            }
        }

        Item {
            width: root.width
            height: root.height - unitBreadcrumb.height - 10

            ListView {
                id: phraseList
                visible: !root.editMode
                anchors.fill: parent
                clip: true
                model: phraseModel
                delegate: itemDelegate

                PlasmaComponents.ScrollBar {
                    flickableItem: phraseList
                }
            }
            PhraseEditor {
                id: phraseEditor
                visible: root.editMode
                width: root.width
                height: parent.height
                onEditorClosed: {
                    root.editMode = false
                }
            }
        }
    }
}
