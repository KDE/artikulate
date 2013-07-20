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

    property TrainingSession session

    signal typeSelected(int type)

    width: 200
    height: 20

    Row {
        height: root.height
        width: root.width

        Item { // word
            height: root.height
            width: root.width/4
            Rectangle {
                color: "#A7E8BD"
                width: parent.width; height: parent.height
            }
            Rectangle {
                color: "#2de86c"
                width: parent.width * session.progressTypeWord/100
                height: parent.height
            }
        }

        Item { // expression
            height: root.height
            width: root.width/4
            Rectangle {
                color: "#B9D5FF"
                width: parent.width; height: parent.height
            }
            Rectangle {
                color: "#327bff"
                width: parent.width * session.progressTypeExpression/100
                height: parent.height
            }
        }

        Item { // sentence
            height: root.height
            width: root.width/4
            Rectangle {
                color: "#F5FFCB"
                width: parent.width; height: parent.height
            }
            Rectangle {
                color: "#fff13f"
                width: parent.width * session.progressTypeSentence/100
                height: parent.height
            }
        }

        Item { // paragraph
            height: root.height
            width: root.width/4
            Rectangle {
                color: "#E8CAB9"
                width: parent.width; height: parent.height
            }
            Rectangle {
                color: "#e85a02"
                width: parent.width * session.progressTypeParagraph/100
                height: parent.height
            }
        }

//         Column {
//             PlasmaComponents.ButtonColumn {
//                 PlasmaComponents.RadioButton {
//                     id: radioWord
//                     text: i18n("Word")
//                     onCheckedChanged: {
//                         if (!checked) return
//                         root.typeString = i18n("Word")
//                         type = Phrase.Word
//                     }
//                 }
//                 PlasmaComponents.RadioButton {
//                     id: radioExpression
//                     text: i18n("Expression")
//                     onCheckedChanged: {
//                         if (!checked) return
//                         root.typeString = i18n("Expression")
//                         type = Phrase.Expression
//                     }
//                 }
//                 PlasmaComponents.RadioButton {
//                     id: radioSentence
//                     text: i18n("Sentence")
//                     onCheckedChanged: {
//                         if (!checked) return
//                         root.typeString = i18n("Sentence")
//                         type = Phrase.Sentence
//                     }
//                 }
//                 PlasmaComponents.RadioButton {
//                     id: radioParagraph
//                     text: i18n("Paragraph")
//                     onCheckedChanged: {
//                         if (!checked) return
//                         root.typeString = i18n("Paragraph")
//                         type = Phrase.Paragraph
//                     }
//                 }
//             }
//         }
    }
}
