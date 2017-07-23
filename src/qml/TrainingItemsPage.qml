/*
 *  Copyright 2013-2017  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *  Copyright 2013       Magdalena Konkiewicz <konkiewicz.m@gmail.com>
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

import QtQuick 2.5
import QtQml.Models 2.2
import QtQuick.Controls 1.4
import org.kde.kirigami 2.0 as Kirigami2
import artikulate 1.0

Kirigami2.Page {
    TreeView {
        id: phraseTree
        anchors.fill: parent
        TableViewColumn {
            title: i18n("Categories")
            role: "text"
        }
        model: PhraseModel {
            id: phraseModel
            course: g_trainingSession.course
        }
        selection: ItemSelectionModel {
            model: phraseTree.model
        }
        itemDelegate: Item {
            property bool isUnit: phraseModel.isUnit(styleData.index)
            Component {
                id: unitRowBackground
                Rectangle {anchors.fill: parent; color: "steelblue"}
            }
            Component {
                id: phraseProgress
                Row {
                    property int progress: isUnit ? 0 : phraseModel.phrase(styleData.index).progress
                    Repeater {
                        model: 3
                        Icon {
                            width: 16
                            height: width
                            icon: progress > index ? "rating" : "rating-unrated"
                        }
                    }
                }
            }
            Loader {
                anchors.fill: parent
                sourceComponent: isUnit ? unitRowBackground : null
            }
            Row {
                Text {
                    width: phraseTree.width - 130 //TODO check if this is really a reasonable value
                    anchors {
                        verticalCenter: parent.verticalCenter
                        topMargin: 5
                        bottomMargin: 5
                    }
                    color: {
                        if (isUnit) {
                            return "white";
                        }
                        return styleData.textColor
                    }
                    elide: Text.ElideRight
                    text: " " + styleData.value
                    font.bold: isUnit
                }
                Loader {
                    sourceComponent: isUnit ? null : phraseProgress
                }
            }
        }
        onClicked: {
            g_trainingSession.phrase = phraseModel.phrase(index)
        }
        Connections {
            target: g_trainingSession
            onPhraseChanged: {
                phraseTree.expand(phraseModel.indexUnit(g_trainingSession.phrase.unit))
                phraseTree.selection.setCurrentIndex(
                    phraseModel.indexPhrase(g_trainingSession.phrase),
                    ItemSelectionModel.ClearAndSelect)
            }
        }
    }
}
