// SPDX-FileCopyrightText: 2013-2025 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import org.kde.artikulate

ScrollView {
    id: root

    required property Phrase phrase
    required property bool isSkeletonPhrase

    ColumnLayout {
        Kirigami.FormLayout {
            TextField {
                id: i18nPhrase
                Kirigami.FormData.label: i18n("Localized Phrase:")
                text: root.phrase.text
                onEditingFinished: {
                    root.phrase.text = text
                }
            }
            ComboBox {
                Kirigami.FormData.label: i18n("Complexity:")
                textRole: "text"
                valueRole: "value"
                onActivated: root.phrase.type = currentValue
                Component.onCompleted: currentIndex = indexOfValue(root.phrase.type)
                model: [
                    { value: Phrase.Word, text: i18n("Word") },
                    { value: Phrase.Expression, text: i18n("Expression") },
                    { value: Phrase.Sentence, text: i18n("Sentence") },
                    { value: Phrase.Paragraph, text: i18n("Paragraph") }
                ]
            }
            ComboBox {
                visible: !root.isSkeletonPhrase
                Kirigami.FormData.label: i18n("Translation State:")
                textRole: "text"
                valueRole: "value"
                onActivated: root.phrase.editState = currentValue
                Component.onCompleted: currentIndex = indexOfValue(root.phrase.editState)
                model: [
                    { value: Phrase.Unknown, text: i18nc("state", "Unknown") },
                    { value: Phrase.Translated, text: i18nc("state", "Translated") },
                    { value: Phrase.Completed, text: i18nc("state", "Completed") }
                ]
            }
        }

        Row {
            width: root.width
            Column {
                id: textEdit
                width: parent.width
                spacing: 5

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
                    model: PhonemeModel {
                        language: EditorSession.language
                    }
                    delegate: phonemeItem
                }
            }
        }
    }
}
