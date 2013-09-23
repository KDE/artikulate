/*
 *  Copyright 2013  Oindrila Gupta <oindrila.gupta92@gmail.com>
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

    property int totalNumberOfPhrasesInUnit
    property int heightForEachPhrase
    property int maxTries
    property int widthForEachBar
    property int hLine1
    property int hLine2
    property bool finished: session.isFinished

    onFinishedChanged: {
        if (finished) {
            computeStatistics()
            statisticsGraphLoader.sourceComponent = statisticsGraph
        }
    }

    Loader {
        id: statisticsGraphLoader
        height: root.height
        width: root.width
        anchors { left: root.left; bottom: root.bottom }
    }

    function computeStatistics() {
        totalNumberOfPhrasesInUnit = session.numberPhrases(Phrase.Word) + session.numberPhrases(Phrase.Expression) + session.numberPhrases(Phrase.Sentence) + session.numberPhrases(Phrase.Paragraph);
        if (totalNumberOfPhrasesInUnit % 10 == 0) {
            highestMark = totalNumberOfPhrasesInUnit;
        }
        else {
            highestMark = totalNumberOfPhrasesInUnit + 10 - (totalNumberOfPhrasesInUnit % 10);
        }
        heightForEachPhrase = (root.height - 30 - 20 - 10 - 10) / highestMark;
        hLine1 = highestMark - 10;
        hLine2 = hLine1 / 2;
        maxTries = session.maximumTries() + 1;
        widthForEachBar = (root.width - 80 - 10) / ((2 * maxTries) + 1);
    }

    Component {
        id: statisticsGraph

        Row {
            spacing: 10
            height: parent.height
            width: parent.width

            Text {
                id: yText
                text: "Phrases"
                font.pointSize: 14
            }

            Column {
                height: parent.height
                width: parent.width - 80 - 10
                anchors.bottom: parent.bottom
                spacing: 10

                Row {
                    height: parent.height - 30 - 20 - 10 - 10
                    width: parent.width
                    anchors.top: parent.top
                    spacing: widthForEachBar

                    Rectangle {
                        id: verticalLine
                        height: parent.height
                        width: 2
                        color: "black"
                        anchors.left: parent.left
                    }

                    Repeater {
                        model: maxTries
                        anchors.bottom: parent.bottom

                        Column {
                            id: statisticsBar
                            height: paragraph.height + sentence.height + expression.height + word.height + (1 / 10)
                            width: widthForEachBar
                            spacing: 0
                            anchors.bottom: parent.bottom

                            Rectangle {
                                id: paragraph
                                height: session.numberPhrasesGroupedByTries(Phrase.Paragraph, index + 1) * heightForEachPhrase
                                width: parent.width
                                color: "#e85a02"
                            }
                            Rectangle {
                                id: sentence
                                height: session.numberPhrasesGroupedByTries(Phrase.Sentence, index + 1) * heightForEachPhrase
                                width: parent.width
                                color: "#fff13f"
                            }
                            Rectangle {
                                id: expression
                                height: session.numberPhrasesGroupedByTries(Phrase.Expression, index + 1) * heightForEachPhrase
                                width: parent.width
                                color: "#327bff"
                            }
                            Rectangle {
                                id: word
                                height: session.numberPhrasesGroupedByTries(Phrase.Word, index + 1) * heightForEachPhrase
                                width: parent.width
                                color: "#2de86c"
                            }
                        }
                    }

                    Rectangle {
                        id: horizontalLine
                        anchors { top: verticalLine.bottom; left: verticalLine.left }
                        height: 2
                        width: parent.width
                        color: "black"
                    }

                    Rectangle {
                        id: horizontalLine1
                        anchors { bottom: horizontalLine.top; bottomMargin: hLine1 * heightForEachPhrase; left: verticalLine.left }
                        height: 1
                        width: parent.width
                        color: "black"
                    }

                    Rectangle {
                        id: horizontalLine2
                        anchors { bottom: horizontalLine.top; bottomMargin: hLine2 * heightForEachPhrase; left: verticalLine.left }
                        height: 1
                        width: parent.width
                        color: "black"
                    }

                    Text {
                        text: hLine1
                        font.pointSize: 14
                        anchors { right: parent.right; bottom: horizontalLine1.top; bottomMargin: 5}
                    }

                    Text {
                        text: hLine2
                        font.pointSize: 14
                        anchors { right: parent.right; bottom: horizontalLine2.top; bottomMargin: 5}
                    }
                }

                Row {
                    id: xAxisMark
                    height: 20
                    width: parent.width - (3 * widthForEachBar)
                    anchors { right: parent.right; rightMargin: widthForEachBar * 3 / 2 }
                    spacing: (widthForEachBar * 2) - 12

                    Repeater {
                        model: maxTries

                        Text {
                            id: tries
                            text: index + 1
                            font.pointSize: 14
                        }
                    }
                }

                Text {
                    id: xText
                    text: "Tries"
                    font.pointSize: 14
                    anchors { right: parent.right }
                }
            }
        }
    }
}
