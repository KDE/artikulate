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
        Item {
            height: 10
            width: root.width
            Rectangle {
                // TODO this is a very simple effect, should be exchanged by real artwork
                // that better illustrates usage of this slider
                id: line
                width: parent.width*0.95
                height: 1
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#000"
            }
            PlasmaComponents.Slider {
                id: typeSlider
                anchors.fill: parent
                minimumValue: 0
                maximumValue: 3
                onValueChanged: {
                    switch (Math.round(value)) {
                        case Phrase.Word:
                            root.typeString = i18n("Word")
                            break;
                        case Phrase.Expression:
                            root.typeString = i18n("Expression")
                            break;
                        case Phrase.Sentence:
                            root.typeString = i18n("Sentence")
                            break;
                        case Phrase.Paragraph:
                            root.typeString = i18n("Paragraph")
                            break;
                        default:
                            root.typeString = i18n("unselected")
                    }
                    typeSelected(Math.round(value))
                }
            }
        }
    }
}
