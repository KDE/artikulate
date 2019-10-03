/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import artikulate 1.0

Item {
    id: root

    property Phrase phrase
    property bool isSkeletonPhrase: false

    // use for saving
    property int __changedPhraseType
    property string __changedPhraseText

    width: 500
    height: editLoader.height

    Component {
        id: editComponent

        Row {
            width: root.width
            height: {
                if (!root.isSkeletonPhrase)
                    textEdit.height + phonemeGrid.height + phraseEditStateSetter.height + phraseRecorder.height + phraseTypeSetter.height;
                else { // height if only editing skeleton
                    textEdit.height + phraseTypeSetter.height;
                }
            }
            ColumnLayout {
                id: textEdit
                height: inputLine.height + originalPhraseInfo.height
                width: parent.width
                spacing: 5

                Row {
                    id: originalPhraseInfo
                    property string originalPhrase : (root.phrase != null) ? root.phrase.i18nText : ""
                    spacing: 10
                    visible: { root.phrase != null && originalPhrase != "" && !root.isSkeletonPhrase}
                    Text {
                        text: i18n("Original Phrase: <i>%1</i>", originalPhraseInfo.originalPhrase)
                        width: root.width - 70
                        wrapMode: Text.WordWrap
                    }
                }
                RowLayout { // controls for setting phrase
                    id: inputLine
                    TextArea {
                        id: phraseInput
                        property Phrase phrase: root.phrase
                        Layout.fillWidth: true
                        Layout.maximumHeight: 100
                        text: root.phrase.text
                        onTextChanged: {
                            if (root.phrase == null) {
                                return
                            }
                            root.phrase.text = text
                        }
                        onPhraseChanged: {
                            if (root.phrase != null)
                                text = root.phrase.text
                            else
                                text = ""
                        }
                    }
                }

                PhraseEditorTypeComponent {
                    id: phraseTypeSetter
                    phrase: root.phrase
                }

                PhraseEditorSoundComponent {
                    id: phraseRecorder
                    visible: !root.isSkeletonPhrase
                    phrase: root.phrase
                }

                Component {
                    id: phonemeItem
                    Text {
                        Button {
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
                            language: g_editorSession.language
                        }
                    delegate: phonemeItem
                }
                RowLayout {
                    id: controls
                    anchors {
                        left: parent.left
                        right: parent.right
                    }
                    PhraseEditorEditStateComponent {
                        id: phraseEditStateSetter
                        visible: !root.isSkeletonPhrase
                        phrase: root.phrase
                    }
                    Label { // dummy
                        Layout.fillWidth: true
                    }
                }
            }
        }
    }

    ColumnLayout {
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
