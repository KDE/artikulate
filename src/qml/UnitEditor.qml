/*
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

    Component {
        id: itemDelegate

        Row {
            id: phraseRow
            spacing: 5
            anchors.centerIn: parent.center
            height: Math.max(phraseText.height, typeIcon.height) + 10

            property Phrase phrase: model.dataRole

            ToolButton {
                id: enableEdit
                iconName: "document-properties"
                enabled: !phrase.excluded
                onClicked: {
                    editMode = true
                    phraseEditor.phrase = phrase
                }
            }

            property bool phraseExcluded: false

            Image {
                id: typeIcon

                width: 32
                height: 32
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

            Rectangle {
                id: coloredBox
                width: theme.smallIconSize
                height: theme.smallIconSize
                anchors.verticalCenter: enableEdit.verticalCenter

                color: {
                    switch (phrase.editState) {
                    case Phrase.Unknown: "red";
                        break;
                    case Phrase.Translated: "orange";
                        break;
                    case Phrase.Completed: "green";
                        break;
                    default: "purple";
                    }
                }
                visible: {
                    if (editCourseSelector.isSkeleton) {
                        visible = false
                    }
                    else {
                        visible = true
                    }
                }
            }

            Text {
                id: phraseText
                width: root.width - enableEdit.width - typeIcon.width - excludeButton.width - coloredBox.width - 40
                anchors.verticalCenter: enableEdit.verticalCenter
                font.italic: phrase.excluded
                text: phrase.text
                wrapMode: Text.WordWrap
            }

            ToolButton {
                id: excludeButton
                iconName: phrase.excluded ? "list-add" : "list-remove"
                onClicked: {
                    if (!phrase.excluded) {
                        unit.excludeSkeletonPhrase(phrase.id)
                    } else {
                        unit.includeSkeletonPhrase(phrase.id)
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

            ToolButton {
                anchors.verticalCenter: unitBreadcrumb.verticalCenter
                text: i18n("Unit: ")
                iconName: "go-up"
                height: 1.5 * theme.fontPointSize
                onClicked: {
                    closeUnit()
                    phraseEditor.close()
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
                font.pointSize: 1.5 * theme.fontPointSize
            }
            TextField {
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
            ToolButton {
                id: enableUnitEdit
                anchors.verticalCenter: unitBreadcrumb.verticalCenter
                iconName: "document-properties"
                enabled: root.unit != null
                visible: !unitBreadcrumb.editMode
                onClicked: {
                    unitBreadcrumb.editMode = !unitBreadcrumb.editMode
                }
            }
            ToolButton {
                anchors.verticalCenter: unitBreadcrumb.verticalCenter
                visible: unitBreadcrumb.editMode
                iconName: "dialog-ok-apply"
                onClicked: {
                    root.unit.title = unitTitleInput.text
                    unitBreadcrumb.editMode = false
                }
            }
            ToolButton {
                anchors.verticalCenter: unitBreadcrumb.verticalCenter
                visible: unitBreadcrumb.editMode
                iconName: "dialog-cancel"
                onClicked: {
                    unitBreadcrumb.editMode = false
                }
            }
        }

        ToolButton {
            id: addPhraseButton
            text: i18n("Add Phrase")
            iconName: "document-new"
            enabled: root.unit != null && !root.editMode
            onClicked: {
                root.unit.course.createPhrase(unit)
            }
        }

        Item {
            width: root.width
            height: root.height - unitBreadcrumb.height - 10

            ScrollView {
                ListView {
                    id: phraseList
                    visible: !root.editMode
                    anchors.fill: parent
                    clip: true
                    model: PhraseFilterModel
                    {
                        hideExcluded: false
                        phraseModel: PhraseModel {
                            unit: root.unit
                        }
                    }
                    delegate: itemDelegate
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
