/*
    SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.7 as Kirigami
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
            Column {
                id: textEdit
                width: parent.width
                spacing: 5

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

        Kirigami.FormLayout {
            Kirigami.Separator {
                Kirigami.FormData.label: g_editorSession.unit.i18nTitle === "" ? i18n("Unit") : i18n("Unit: ") + g_editorSession.unit.i18nTitle
                Kirigami.FormData.isSection: true
            }
            TextField {
                id: i18nUnit
                Kirigami.FormData.label: i18n("Localized Unit:")
                text: g_editorSession.unit.title
                onEditingFinished: {
                    g_editorSession.unit.title = text
                }
                Connections {
                    target: root
                    onPhraseChanged: i18nUnit.text = Qt.binding(function() { return g_editorSession.unit.title })
                }
            }
            Kirigami.Separator {
                Kirigami.FormData.label: i18n("Phrase: ") + root.phrase.i18nText
                Kirigami.FormData.isSection: true
            }
            TextField {
                id: i18nPhrase
                Kirigami.FormData.label: i18n("Localized Phrase:")
                text: root.phrase.text
                onEditingFinished: {
                    root.phrase.text = text
                }
                Connections {
                    target: root
                    onPhraseChanged: i18nPhrase.text = Qt.binding(function() { return root.phrase.i18nText })
                }
            }
        }

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
