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
import artikulate 1.0

Item {
    id: root

    property Phrase phrase

    // use for saving
    property int __changedPhraseType
    property int __changedPhraseEditState
    property string __changedPhraseText


    signal editorClosed()

    width: 500
    height: editLoader.height

    onPhraseChanged: {
        if (root.phrase) {
            root.__changedPhraseType = root.phrase.type
            root.__changedPhraseEditState = root.phrase.editState
            root.__changedPhraseText = root.phrase.text
        }
    }

    function close()
    {
        root.phrase = null
        editorClosed()
    }

    function applyChanges()
    {
        root.phrase.type = root.__changedPhraseType
        root.phrase.editState = root.__changedPhraseEditState
        root.phrase.text = root.__changedPhraseText
    }

    Component {
        id: editComponent

        Row {
            width: root.width
            height: {
                if (!editCourseSelector.isSkeleton)
                    textEdit.height + phonemeGrid.height + phraseEditStateSetter.height + phraseRecorder.height + phraseTypeSetter.height;
                else { // height if only editing skeleton
                    textEdit.height + phraseTypeSetter.height;
                }
            }
            Column {
                id: textEdit
                height: inputLine.height + originalPhraseInfo.height
                spacing: 5
                Row { // controls for setting phrase
                    id: inputLine
                    height: 30
                    PlasmaComponents.TextField {
                        id: phraseInput
                        property Phrase phrase : root.phrase
                        width: root.width - buttonAccept.width - buttonCancel.width - 15
                        text: root.phrase.text
                        anchors.verticalCenter: inputLine.verticalCenter
                        onTextChanged: root.__changedPhraseText = text
                        onPhraseChanged: {
                            if (root.phrase != null)
                                text = root.phrase.text
                            else
                                text = ""
                        }
                        onAccepted: {
                            applyChanges()
                            close()
                        }
                    }
                    PlasmaComponents.ToolButton {
                        id: buttonAccept
                        width: 48
                        height: 48
                        anchors.verticalCenter: inputLine.verticalCenter
                        iconSource: "dialog-ok-apply"
                        onClicked: {
                            applyChanges()
                            close()
                        }
                    }
                    PlasmaComponents.ToolButton {
                        id: buttonCancel
                        width: 48
                        height: 48
                        anchors.verticalCenter: inputLine.verticalCenter
                        iconSource: "dialog-cancel"
                        onClicked: {
                            phraseInput.text = root.phrase.text
                            phraseEditStateSetter.updateCheckedStates()
                            phraseTypeSetter.updateCheckedStates()
                            close()
                        }
                    }
                }
                Row {
                    id: originalPhraseInfo
                    property string originalPhrase : (root.phrase != null) ? root.phrase.i18nText : ""
                    spacing: 10
                    visible: { root.phrase != null && originalPhrase != "" }
                    Text {
                        text: i18n("Original Phrase:") + " <i>" + originalPhraseInfo.originalPhrase + "</i>"
                        width: root.width - 70
                        wrapMode: Text.WordWrap
                    }
                }

                PhraseEditorEditStateComponent {
                    id: phraseEditStateSetter
                    visible: !editCourseSelector.isSkeleton
                    phrase: root.phrase
                    onSelectedEditStateChanged: root.__changedPhraseEditState = selectedEditState
                }

                PhraseEditorTypeComponent {
                    id: phraseTypeSetter
                    phrase: root.phrase
                    onSelectedTypeChanged: root.__changedPhraseType = selectedType
                }

                PhraseEditorSoundComponent {
                    id: phraseRecorder
                    visible: !editCourseSelector.isSkeleton
                    phrase: root.phrase
                }

                Component {
                    id: phonemeItem
                    Text {
                        PlasmaComponents.Button {
                            width: 100
                            text: model.title
                            checkable: true
                            checked: { phrase != null && phrase.hasPhoneme(model.dataRole) }
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
                    width: root.width
                    height: 30 * count / columns + 60
                    cellWidth: 100
                    cellHeight: 30
                    model:
                        PhonemeModel {
                            language: { (phrase != null) ? root.phrase.unit.course.language : null }
                        }
                    delegate: phonemeItem
                }
            }
        }
    }

    Row {
        id: phraseRow

        Loader {
            id: editLoader
            sourceComponent: (phrase != null) ? editComponent : undefined
            onSourceComponentChanged: {
                if (sourceComponent == undefined) height = 0
                else height = editComponent.height
            }
        }
    }
}
