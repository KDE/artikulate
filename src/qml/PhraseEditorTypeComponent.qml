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

    width: typeController.width
    height: typeController.height

    Column {
        id: typeController

        Text {
            id: componentTitle
            text: i18n("Phrase Type")
            font.pointSize: 14;
        }

        PlasmaComponents.ButtonRow { // controls for setting type
            id: phraseTypeRow
            spacing: 30

            anchors { left: componentTitle.left; leftMargin: 30 }

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
