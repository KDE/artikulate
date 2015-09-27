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

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import artikulate 1.0

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

        RowLayout {
            ExclusiveGroup { id: editTypeGroup }
            RadioButton {
                id: buttonWord
                text: i18n("Word")
                onClicked: {
                    root.phrase.type = Phrase.Word
                }
                exclusiveGroup: editTypeGroup
            }
            RadioButton {
                id: buttonExpression
                text: i18n("Expression")
                onClicked: {
                    root.phrase.type = Phrase.Expression
                }
                exclusiveGroup: editTypeGroup
            }
            RadioButton {
                id: buttonSentence
                text: i18n("Sentence")
                onClicked: {
                    root.phrase.type = Phrase.Sentence
                }
                exclusiveGroup: editTypeGroup
            }
            RadioButton {
                id: buttonParagraph
                text: i18n("Paragraph")
                onClicked: {
                    root.phrase.type = Phrase.Paragraph
                }
                exclusiveGroup: editTypeGroup
            }
        }
    }
}
