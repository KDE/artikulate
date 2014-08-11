/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *  Copyright 2013  Samikshan Bairagya <samikshan@gmail.com>
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
        trainingText.text= i18n("Category: %1", phraseTypeString)
    }

    width: 1500
    height: 20

    Component {
        id: progressBarDelegate

        Item {
            id: phraseItem
            width: root.width / progressFooModel.filteredCount
            height: 20

            // The Z value for the item is set to the width of the rectangle.
            // This hack keeps the bigger phrase box above other boxes top.
            z: phraseBox.width
            Rectangle {
                id: phraseBox
                property Phrase currentPhrase : model.dataRole
                property bool isSelected : boxMouseArea.containsMouse
                signal phraseSelected()

                width: boxMouseArea.containsMouse ? parent.width + 15 : parent.width
                height: boxMouseArea.containsMouse ? 35 : 20
                border.width: boxMouseArea.containsMouse ? 3 : 1
                anchors.centerIn: parent

                color: {
                    switch(currentPhrase.type) {
                        case Phrase.Word:
                            currentPhrase.trainingState == Phrase.Untrained ? "#A7E8BD" : "#2de86c"
                            break;
                        case Phrase.Expression:
                            currentPhrase.trainingState == Phrase.Untrained ? "#B9D5FF" : "#327bff"
                            break;
                        case Phrase.Sentence:
                            currentPhrase.trainingState == Phrase.Untrained ? "#F5FFCB" : "#fff13f"
                            break;
                        case Phrase.Paragraph:
                            currentPhrase.trainingState == Phrase.Untrained ? "#E8CAB9" : "#e85a02"
                            break;
                        default:
                            break;
                    }
                }

                onPhraseSelected: {
                    session.jumpToPhrase(currentPhrase)
                    switch(currentPhrase.type) {
                        case Phrase.Word:
                            session.setPhraseType("word")
                            break;
                        case Phrase.Expression:
                            session.setPhraseType("expression")
                            break;
                        case Phrase.Sentence:
                            session.setPhraseType("sentence")
                            break;
                        case Phrase.Paragraph:
                            session.setPhraseType("paragraph")
                            break;
                        default:
                            break;
                    }
                }

                border.color: Qt.darker(phraseBox.color, 1.2)

                Behavior on width {PropertyAnimation {}}
                Behavior on height {PropertyAnimation {}}

                MouseArea {
                    id: boxMouseArea
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        phraseBox.phraseSelected()
                    }
                }
            }

            Item {
                id: textItem
                anchors {
                    bottom: phraseBox.verticalCenter
                    bottomMargin: 40
                }
                x: {
                    if (phraseItem.x - textItem.width/2 < 0) {
                        return 0
                    }
                    if (phraseItem.x + textItem.width/2 > progressView.width) {
                        return (progressView.width - phraseItem.x) - textItem.width
                    }
                    return - Math.floor(textItem.width/2) + phraseItem.width/2
                }
                width: phraseText.width
                height: phraseText.height

                Text {
                    id: phraseText
                    wrapMode: Text.WordWrap
                    anchors.centerIn: parent
                    visible: phraseBox.isSelected
                    text: phraseBox.currentPhrase.text
                    font.italic: true
                    horizontalAlignment: (phraseItem.x > root.width/2) ? Text.AlignRight : Text.AlignLeft
                    Component.onCompleted: {
                        if (phraseText.paintedWidth > root.width) {
                            phraseText.width = root.width
                        }
                    }
                }
            }
        }
    }

    ListView {
        id: progressView
        anchors.fill: parent
        orientation: ListView.Horizontal
        delegate: progressBarDelegate
        model: PhraseFilterModel {
            id: progressFooModel
            hideNotRecorded: true
            hideExcluded: true
            sortOption: PhraseFilterModel.Type
            phraseModel: PhraseModel {
                id: progressBarModel
                unit: userProfile.unit
            }
        }
    }
}
