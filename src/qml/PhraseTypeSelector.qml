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
    width: 100
    height: 30

    property int type
    property string typeString
    signal typeSelected(int type)

    Column {
        spacing: 10
        Text {
            text : i18n("<b>Difficulty:</b> %1", typeString)
        }
        Image {
            width: 64
            height: 64
            source: "../icons/hicolor/64x64/actions/artikulate-word.png"
        }
        Image {
            width: 64
            height: 64
            source: "../icons/hicolor/64x64/actions/artikulate-expression.png"
        }
        Image {
            width: 64
            height: 64
            source: "../icons/hicolor/64x64/actions/artikulate-sentence.png"
        }
        Image {
            width: 64
            height: 64
            source: "../icons/hicolor/64x64/actions/artikulate-paragraph.png"
        }
    }
    Column {
        x: 70
        y: 44
        PlasmaComponents.ButtonColumn {
            spacing: 52
            PlasmaComponents.RadioButton {
                id: radioWord
                text: i18n("Word")
                onCheckedChanged: {
                    if (!checked) return
                    root.typeString = i18n("Word")
                    typeSelected(Phrase.Word)
                }
            }
            PlasmaComponents.RadioButton {
                id: radioExpression
                text: i18n("Expression")
                onCheckedChanged: {
                    if (!checked) return
                    root.typeString = i18n("Expression")
                    typeSelected(Phrase.Expression)
                }
            }
            PlasmaComponents.RadioButton {
                id: radioSentence
                text: i18n("Sentence")
                onCheckedChanged: {
                    if (!checked) return
                    root.typeString = i18n("Sentence")
                    typeSelected(Phrase.Sentence)
                }
            }
            PlasmaComponents.RadioButton {
                id: radioParagraph
                text: i18n("Paragraph")
                onCheckedChanged: {
                    if (!checked) return
                    root.typeString = i18n("Paragraph")
                    typeSelected(Phrase.Paragraph)
                }
            }
        }
    }
}
