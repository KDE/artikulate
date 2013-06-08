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
    property int __originalPhraseType

    width: 500
    height: 30 + editLoader.height

    Component.onCompleted: {
        root.__originalPhraseType = root.phrase.type
    }

    Component {
        id: editComponent

        Row {
            width: root.width - enableEdit.width - typeIcon.width - 20
            height: textEdit.height + phonemeGrid.height + phraseEditStateSetter.height + phraseRecorder.height + phraseTypeSetter.height
            Column {
                id: textEdit
                height: inputLine.height + originalPhraseInfo.height
                spacing: 5
                Row { // controls for setting phrase
                    id: inputLine
                    height: 30
                    PlasmaComponents.TextField {
                        id: phraseInput
                        width: Math.min( Math.max(phraseText.width + 20, 200), root.width - 120)
                        text: root.phrase.text
                        onAccepted: {
                            root.editMode = false
                            root.phrase.text = text
                            root.__originalPhraseType = phrase.type
                        }
                    }
                    PlasmaComponents.ToolButton {
                        iconSource: "dialog-ok-apply"
                        onClicked: {
                            root.editMode = false
                            root.phrase.text = phraseInput.text
                            root.__originalPhraseType = phrase.type
                        }
                    }
                    PlasmaComponents.ToolButton {
                        iconSource: "dialog-cancel"
                        onClicked: {
                            root.editMode = false
                            phrase.type = root.__originalPhraseType
                        }
                    }
                }
                Row {
                    id: originalPhraseInfo
                    spacing: 10
                    visible: { root.phrase.i18nText != "" }
                    Text {
                        text: i18n("Original Phrase:") + " <i>" + root.phrase.i18nText + "</i>"
                        width: root.width - 70
                        wrapMode: Text.WordWrap
                    }
                }

                PhraseEditorEditStateComponent {
                    id: phraseEditStateSetter
                    phrase: root.phrase
                }

                PhraseEditorTypeComponent {
                    id: phraseTypeSetter
                    phrase: root.phrase
                }

                PhraseEditorSoundComponent {
                    id: phraseRecorder
                    phrase: root.phrase
                }

                Component {
                    id: phonemeItem
                    Text {
                        PlasmaComponents.Button {
                            width: 100
                            text: model.title
                            checkable: true
                            checked: phrase.hasPhoneme(model.dataRole)
                            onClicked: { //TODO this button has no undo operation yet
                                if (checked) {
                                    phrase.addPhoneme(model.dataRole)
                                } else {
                                    phrase.removePhoneme(model.dataRole)
                                }
                            }
                        }
                    }
                }

                GridView {
                    id: phonemeGrid
                    property int columns : width / cellWidth
                    width: parent.width
                    height: 30 * count / columns + 60
                    cellWidth: 100
                    cellHeight: 30
                    model: PhonemeModel { language: root.phrase.unit.course.language }
                    delegate: phonemeItem
                }
            }
        }
    }

    Row {
        id: phraseRow
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
            width: root.width - enableEdit.width - typeIcon.width - 20
            anchors.verticalCenter: enableEdit.verticalCenter
            visible: { !root.editMode }
            text: root.phrase.text
            wrapMode: Text.WordWrap
            color: {
                switch (root.phrase.editState) {
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

        Loader {
            id: editLoader
            sourceComponent: editMode ? editComponent : undefined
            onSourceComponentChanged: {
                if (sourceComponent == undefined) height = 0
                else height = editComponent.height
            }
        }
    }
}
