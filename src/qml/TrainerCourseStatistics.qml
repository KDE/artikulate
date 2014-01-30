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
import org.kde.charts 0.1 as Charts
import artikulate 1.0

Item {
    id: root

    property TrainingSession session
    property bool finished : session.isFinished

    onFinishedChanged : {
        if (finished) {
            statisticsChartLoader.sourceComponent = statisticsChart
        }
    }

    Loader {
        id: statisticsChartLoader
        height: Math.round(root.height - 60)
        width: Math.round(root.width - 20)
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
        id : statisticsChart
        Column {
            Row {
                spacing: 15
                width: legendWord.width + legendExpression.width + legendSentence.width + legendParagraph.width + 3*15
                height:legendWord.height*2
                Item { // dummy
                    width: legendWord.height
                    height: legendWord.height
                }
                Charts.LegendItem {
                    id: legendWord
                    anchors.verticalCenter: parent.verticalCenter
                    dimension: wordDimension
                }
                Charts.LegendItem {
                    id: legendExpression
                    anchors.verticalCenter: parent.verticalCenter
                    dimension: expressionDimension
                }
                Charts.LegendItem {
                    id: legendSentence
                    anchors.verticalCenter: parent.verticalCenter
                    dimension: sentenceDimension
                }
                Charts.LegendItem {
                    id: legendParagraph
                    anchors.verticalCenter: parent.verticalCenter
                    dimension: paragraphDimension
                }
            }

            Charts.LineChart {
                height: Math.round(root.height - 60)
                width: Math.round(root.width - 20)
                padding: 20
                model: LearningProgressModel {
                    id: statistics
                    session: root.session
                }
                pitch: 60
    //             textRole: 3 // Qt::ToolTipRole

                dimensions: [
                    // words
                    Charts.Dimension {
                        id: wordDimension
                        dataColumn: 0
                        color: "#2DE86C"
                        maximumValue: Math.max(0, Math.ceil(statistics.maximumPhrasesPerTry / 4) * 4) + 5
                        label: i18n("Words")
                    },
                    // expressions
                    Charts.Dimension {
                        id: expressionDimension
                        dataColumn: 1
                        color: "#327BFF"
                        maximumValue: Math.max(0, Math.ceil(statistics.maximumPhrasesPerTry / 4) * 4) + 5
                        label: i18n("Expressions")
                    },
                    // sentences
                    Charts.Dimension {
                        id: sentenceDimension
                        dataColumn: 2
                        color: "#FFF13F"
                        maximumValue: Math.max(0, Math.ceil(statistics.maximumPhrasesPerTry / 4) * 4) + 5
                        label: i18n("Sentences")
                    },
                    // paragraphs
                    Charts.Dimension {
                        id: paragraphDimension
                        dataColumn: 3
                        color: "#E85A02"
                        maximumValue: Math.max(0, Math.ceil(statistics.maximumPhrasesPerTry / 4) * 4) + 5
                        label: i18n("Paragraphs")
                    }
                ]
            }
        }
    }
}
