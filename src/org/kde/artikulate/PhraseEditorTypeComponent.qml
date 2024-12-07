// SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import org.kde.artikulate

Item {
    id: root

    property Phrase phrase

    width: typeController.width
    height: typeController.height

    Component.onCompleted: {
        updateCheckedStates();
    }

    onPhraseChanged: {
        updateCheckedStates()
    }

    function updateCheckedStates()
    {
        if (root.phrase == null) {
            return;
        }
        switch (root.phrase.type) {
        case Phrase.Word:
            buttonWord.checked = true;
            break;
        case Phrase.Expression:
            buttonExpression.checked = true;
            break;
        case Phrase.Sentence:
            buttonSentence.checked = true;
            break;
        case Phrase.Paragraph:
            buttonParagraph.checked = true;
            break;
        }
    }

    GroupBox {
        id: typeController
        title: i18n("Difficulty:")

        ButtonGroup {
            buttons: editTypesList.children
        }

        RowLayout {
            id: editTypesList
            RadioButton {
                id: buttonWord
                text: i18n("Word")
                onClicked: {
                    root.phrase.type = Phrase.Word
                }
            }
            RadioButton {
                id: buttonExpression
                text: i18n("Expression")
                onClicked: {
                    root.phrase.type = Phrase.Expression
                }
            }
            RadioButton {
                id: buttonSentence
                text: i18n("Sentence")
                onClicked: {
                    root.phrase.type = Phrase.Sentence
                }
            }
            RadioButton {
                id: buttonParagraph
                text: i18n("Paragraph")
                onClicked: {
                    root.phrase.type = Phrase.Paragraph
                }
            }
        }
    }
}
