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

Item {
    id: root

    property Phrase phrase
    property bool editMode: false
    property string __originalPhraseText
    property int __originalPhraseType

    height: root.editMode ? 60 : 30

    Component.onCompleted: {
        root.__originalPhraseText = root.phrase.text
        root.__originalPhraseType = root.phrase.type
    }

    Row {
        spacing: 5
        anchors.centerIn: parent.center

        PlasmaComponents.ToolButton {
            id: enableEdit
            iconSource: "document-properties"
            enabled: {!root.editMode}
            onClicked: {
                root.editMode = !root.editMode
            }
        }

        Image {
            id: typeIcon

            width: 20
            height: 20
            source: {
                switch (root.phrase.type) {
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
            anchors.verticalCenter: enableEdit.verticalCenter
            visible: { !root.editMode }
            text: root.phrase.text
        }

        // line in edit mode
        Column {
            visible: root.editMode == true
            spacing: 5
            Row {
                PlasmaComponents.TextField {
                    id: phraseInput
                    width: Math.max(phraseText.width + 20, 200)
                    text: root.phrase.text
                    onTextChanged: {
                        root.phrase.text = text
                    }
                    onAccepted: {
                        root.editMode = false
                        root.__originalPhraseText = phrase.text
                        root.__originalPhraseType = phrase.type
                    }
                }
                PlasmaComponents.ToolButton {
                    iconSource: "dialog-ok-apply"
                    onClicked: {
                        root.editMode = false
                        root.__originalPhraseText = phrase.text
                        root.__originalPhraseType = phrase.type
                    }
                }
                PlasmaComponents.ToolButton {
                    iconSource: "dialog-cancel"
                    onClicked: {
                        root.editMode = false
                        phrase.text = root.__originalPhraseText
                        phrase.type = root.__originalPhraseType
                    }
                }
            }
            PlasmaComponents.ButtonRow {
                id: phraseTypeRow
                spacing: 30
                Text {
                    anchors.verticalCenter: phraseTypeRow.verticalCenter
                    text: i18n("Type:")
                }
                PlasmaComponents.RadioButton {
                    anchors.verticalCenter: phraseTypeRow.verticalCenter
                    text: i18n("Word")
                    checked: {phrase.type == Phrase.Word}
                    onClicked: {
                        phrase.type = Phrase.Word
                    }
                }
                PlasmaComponents.RadioButton {
                    anchors.verticalCenter: phraseTypeRow.verticalCenter
                    text: i18n("Expression")
                    checked: {phrase.type == Phrase.Expression}
                    onClicked: {
                        phrase.type = Phrase.Expression
                    }
                }
                PlasmaComponents.RadioButton {
                    anchors.verticalCenter: phraseTypeRow.verticalCenter
                    text: i18n("Sentence")
                    checked: {phrase.type == Phrase.Sentence}
                    onClicked: {
                        phrase.type = Phrase.Sentence
                    }
                }
                PlasmaComponents.RadioButton {
                    anchors.verticalCenter: phraseTypeRow.verticalCenter
                    text: i18n("Paragraph")
                    checked: {phrase.type == Phrase.Paragraph}
                    onClicked: {
                        phrase.type = Phrase.Paragraph
                    }
                }
            }
        }
    }
}
