/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQml.Models 2.2
import org.kde.kquickcontrolsaddons 2.0
import artikulate 1.0

Item {
    id: root
    width: 400 //parent.width
    height: 600 //parent.height

    signal downloadNewStuff();

    Item {
        id: theme
        property string backgroundColor: "#ffffff"
        property int smallIconSize: 18
        property int smallMediumIconSize: 22
        property int mediumIconSize: 32
        property int fontPointSize: 11
    }

    property Learner learner: profileManager.activeProfile
    property ResourceManager resourceManager: g_resourceManager

    Component.onCompleted: {
        var learner = profileManager.activeProfile;
        if (learner == null) {
            return;
        }
        g_trainingSession.language = g_resourceManager.language(learner.activeGoal(Learner.Language))
    }

    CourseModel {
        id: availableCourseModel
        resourceManager: g_resourceManager
    }

    ColumnLayout {
        id: main
        spacing: 20
        anchors {
            fill: parent
            topMargin: 30
            leftMargin: 30
            rightMargin: 30
            bottomMargin: 30 + langIcon.height//FIXME workaround
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: langIcon.height
            spacing: 20
            QIconItem {
                id: langIcon
                icon: "artikulate-language"
                width: 48
                height: 48
            }

            ComboBox {
                id: comboLanguage
                Layout.minimumWidth: 200
                model: LanguageModel {
                    id: languageModel
                    resourceModel: LanguageResourceModel {
                        resourceManager: g_resourceManager
                    }
                }
                textRole: "title"
                onCurrentIndexChanged: {
                    if (languageModel.language(currentIndex)) {
                        g_trainingSession.language = languageModel.language(currentIndex)
                    }
                }
            }

            ComboBox {
                id: comboCourse
                Layout.minimumWidth: 200
                model: CourseModel {
                    id: courseModel
                    resourceManager: g_resourceManager
                    language: g_trainingSession.language
                    onLanguageChanged: {
                        if (courseModel.course(0)) {
                            g_trainingSession.course = courseModel.course(0)
                        }
                    }
                }
                textRole: "title"
                onCurrentIndexChanged: {
                    if (courseModel.course(currentIndex)) {
                        g_trainingSession.course = courseModel.course(currentIndex)
                    }
                }
            }

            FlatButton {
                id: knsDownloadButton
                text: i18n("Download")
                iconName: "get-hot-new-stuff"
                iconSize: 22
                fontSize: 12
                anchors {
                    verticalCenter: parent.verticalCenter
                }
                onClicked: downloadNewStuff()
            }
        }

        RowLayout {
            id: mainRow
            spacing: 30

            TreeView {
                id: phraseTree
                Layout.preferredWidth: Math.floor(main.width * 0.3)
                Layout.fillHeight: true
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
                    Loader {
                        anchors.fill: parent
                        sourceComponent: isUnit ? unitRowBackground : null
                    }
                    Text {
                        width: phraseTree.width - 100 //TODO check if this is really a reasonable value
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

            TrainerSessionScreen {
                id: trainerMain
                Layout.alignment: Qt.AlignTop
                Layout.preferredWidth: Math.floor(main.width * 0.7) - 30
                Layout.fillHeight: true
            }
        }

    }

    //FIXME setup dialog deactivated for refactoring
//     SheetDialog {
//         id: profileSelectorSheet
//         anchors {
//             top: root.top
//             topMargin: header.height
//             left: root.left
//             bottom: root.bottom
//             right: root.right
//         }
//         content: ProfileSelector {
//             anchors.fill: parent
//             onProfileChosen: {
//                 profileSelectorSheet.close()
//             }
//         }
//     }

}
