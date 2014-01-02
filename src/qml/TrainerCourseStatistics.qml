/*
 *  Copyright 2013  Oindrila Gupta <oindrila.gupta92@gmail.com>
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
import org.kde.graphs 0.1 as Graphs
import artikulate 1.0

Item {
    id: root

    property TrainingSession session
    property bool finished : session.isFinished

    onFinishedChanged : {
        if (finished) {
            statisticsGraphLoader.sourceComponent = statisticsGraph
        }
    }

    Loader {
        id: statisticsGraphLoader
        // add padding
        // TODO should become parameter in garph class
        height : Math.round(root.height - 60)
        width : Math.round(root.width - 20)
        anchors { top: root.top; topMargin: 10; left: root.left; leftMargin: 10 }
    }

    Rectangle {
        id: backgroundRect
        anchors.fill: parent
        z: -1
        color: "#ffffff"
    }

    Text {
        anchors {
            bottom: backgroundRect.bottom
            bottomMargin: 10
            horizontalCenter: backgroundRect.horizontalCenter
        }
        text: i18n("Needed Tries")
    }

    Component {
        id : statisticsGraph
        Column {
            Row {
                spacing: 15
                width: legendWord.width + legendExpression.width + legendSentence.width + legendParagraph.width + 3*15
                anchors { right: parent.right; top: parent.top}
                Graphs.LegendItem {
                    id: legendWord
                    dimension: wordDimension
                }
                Graphs.LegendItem {
                    id: legendExpression
                    dimension: expressionDimension
                }
                Graphs.LegendItem {
                    id: legendSentence
                    dimension: sentenceDimension
                }
                Graphs.LegendItem {
                    id: legendParagraph
                    dimension: paragraphDimension
                }
            }


            Graphs.LineGraph {
                height : Math.round(root.height - 60)
                width : Math.round(root.width - 20)
                model: LearningProgressModel {
                    id: statistics
                    session: root.session
                }
                pitch: 60
    //             textRole: 3 // Qt::ToolTipRole

                dimensions: [
                    // words
                    Graphs.Dimension {
                        id: wordDimension
                        dataColumn : 0
                        color: "#2DE86C"
                        maximumValue: Math.max(0, Math.ceil(statistics.maximumPhrasesPerTry / 4) * 4) + 5
                        label: i18n("Words")
                    },
                    // expressions
                    Graphs.Dimension {
                        id: expressionDimension
                        dataColumn: 1
                        color: "#327BFF"
                        maximumValue: Math.max(0, Math.ceil(statistics.maximumPhrasesPerTry / 4) * 4) + 5
                        label: i18n("Expressions")
                    },
                    // sentences
                    Graphs.Dimension {
                        id: sentenceDimension
                        dataColumn: 2
                        color: "#FFF13F"
                        maximumValue: Math.max(0, Math.ceil(statistics.maximumPhrasesPerTry / 4) * 4) + 5
                        label: i18n("Sentences")
                    },
                    // paragraphs
                    Graphs.Dimension {
                        id: paragraphDimension
                        dataColumn: 3
                        color: "#E85A02"
                        maximumValue: Math.max(0, Math.ceil(statistics.maximumPhrasesPerTry / 4) * 4) + 5
                        label: i18n("Paragraphs")
                    }
                ]

//             onBarEntered: {
//                 errorsTooltip.visualParent = bar;
//                 errorsTooltip.row = row
//                 errorsTooltip.open()
//             }
//
//             onBarExited: {
//                 errorsTooltip.close()
//             }
            }
        }
    }
}
