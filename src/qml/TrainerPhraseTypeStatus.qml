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
    property int sessionType
    property string phraseTypeString
    property int __highlightWidth: 4

    signal typeSelected(int type)

    onSessionTypeChanged: {
        if (sessionType == Phrase.Word){
            phraseTypeString = "Words"
        }
        if (sessionType == Phrase.Expression){
            phraseTypeString = "Expressions"
        }
        if (sessionType == Phrase.Sentence){
            phraseTypeString = "Sentences"
        }
        if (sessionType == Phrase.Paragraph){
            phraseTypeString = "Paragraphs"
        }
        trainingText.text= i18n("Difficulty: %1", phraseTypeString)
    }

    width: 200
    height: 20

    Row {
        id: statusBar
        height: root.height
        width: root.width
        spacing: 0

        Rectangle { // word
            id: word
            height: root.height
            width: root.width/4
            border.color: "#800000FF"
            border.width: sessionType == Phrase.Word ? __highlightWidth : 0
            z: sessionType == Phrase.Word ? 1 : 0
            Rectangle {
                color: "#A7E8BD"
                width: parent.width; height: parent.height
            }
            Rectangle {
                color: "#2de86c"
                width: parent.width * session.progressTypeWord/100
                height: parent.height
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                    session.setPhraseType("word")
                }
                onEntered: {
                    if (phraseTypeString != "Words") {
                        trainingText.text= i18n("Difficulty: <i>change to Words</i>")
                    }
                }
                onExited: {
                    trainingText.text= i18n("Difficulty: %1", phraseTypeString)
                }
            }
        }
        Rectangle { // expression
            id: expression
            height: root.height
            width: root.width/4
            border.color: "#800000FF"
            border.width: sessionType == Phrase.Expression ? __highlightWidth : 0
            z: sessionType == Phrase.Expression ? 1 : 0
            Rectangle {
                color: "#B9D5FF"
                width: parent.width; height: parent.height
            }
            Rectangle {
                color: "#327bff"
                width: parent.width * session.progressTypeExpression/100
                height: parent.height
            }
             MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                        session.setPhraseType("expression")
                }
                onEntered: {
                    if (phraseTypeString != "Expressions") {
                        trainingText.text= i18n("Difficulty: <i>change to Expressions</i>")
                    }
                }
                onExited: {
                    trainingText.text= i18n("Difficulty: %1", phraseTypeString)
                }
            }
        }

        Rectangle { // sentence
            id: sentence
            border.color: "#800000FF"
            border.width: sessionType == Phrase.Sentence ? __highlightWidth : 0
            z: sessionType == Phrase.Sentence ? 1 : 0
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
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                        session.setPhraseType("sentence")
                }
                onEntered: {
                    if (phraseTypeString != "Sentences") {
                        trainingText.text= i18n("Difficulty: <i>change to Sentences</i>")
                    }
                }
                onExited: {
                    trainingText.text= i18n("Difficulty: %1", phraseTypeString)
                }
            }
        }

        Rectangle { // paragraph
            id: paragraph
            height: root.height
            width: root.width/4
            border.color: "#800000FF"
            border.width: sessionType == Phrase.Paragraph ? __highlightWidth : 0
            z: sessionType == Phrase.Paragraph ? 1 : 0
            Rectangle {
                color: "#E8CAB9"
                width: parent.width; height: parent.height
            }
            Rectangle {
                color: "#e85a02"
                width: parent.width * session.progressTypeParagraph/100
                height: parent.height
            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onClicked: {
                        session.setPhraseType("paragraph")
                }
                onEntered: {
                    if (phraseTypeString != "Paragraphs") {
                        trainingText.text= i18n("Difficulty: <i>change to Paragraphs</i>")
                    }
                }
                onExited: {
                    trainingText.text= i18n("Difficulty: %1", phraseTypeString)
                }
            }
        }
    }
}
