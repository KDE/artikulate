/*
 *  Copyright 2013-2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import QtQml.Models 2.2
import org.kde.kquickcontrolsaddons 2.0
import artikulate 1.0

ApplicationWindow {
    id: root
    visible: true
    minimumWidth: 800
    minimumHeight: 600

    signal triggerDownloadCourses();
    signal triggerEditProfile();
    signal triggerSettingsDialog();
    signal triggerAction(string actionName);
    signal switchMenuBarVisibility();

    onTriggerEditProfile: {
        editProfileDialog.open()
    }

    Item {
        id: theme
        property string backgroundColor: "#ffffff"
        property int smallIconSize: 18
        property int smallMediumIconSize: 22
        property int mediumIconSize: 32
        property int fontPointSize: 11
        property int spacing: 15
    }

    property Learner learner: g_profileManager.activeProfile
    property ResourceManager resourceManager: g_resourceManager

    CourseModel {
        id: availableCourseModel
        resourceManager: g_resourceManager
    }

    // main menu bar
    MainMenuBar {
        id: topMenu
    }
    menuBar: { kcfg_ShowMenuBar ? topMenu : null }

    Component {
        id: welcomeScreen
        NewUserWelcome { }
    }

    Component {
        id: trainingScreen
        RowLayout {
            id: mainRow
            Layout.fillHeight: true
            spacing: theme.spacing

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
                    Component {
                        id: phraseProgress
                        Row {
                            property int progress: isUnit ? 0 : phraseModel.phrase(styleData.index).progress
                            Repeater {
                                model: 3
                                QIconItem {
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

            TrainerSessionScreen {
                id: trainerMain
                Layout.alignment: Qt.AlignTop
                Layout.preferredWidth: Math.floor(main.width * 0.7) - 30
                Layout.fillHeight: true
            }
        }
    }

    MainToolBar {
        id: mainToolBar
    }

    ColumnLayout {
        id: main
        spacing: theme.spacing
        anchors {
            fill: parent
            topMargin: mainToolBar.height + theme.spacing
            leftMargin: theme.spacing
            rightMargin: theme.spacing
            bottomMargin: theme.spacing
        }

        Loader {
            Layout.fillHeight: true
            sourceComponent: {
                g_trainingSession.language == null && courseFilterModel.filteredCount == 0
                    ? welcomeScreen
                    : trainingScreen
            }
        }
    }

    //TODO change to loader for arbitrary dialogs
    //TODO add margin for main menu
    SheetDialog {
        id: editProfileDialog
        anchors {
            fill: main
        }
        content: ProfileSettingsItem {
            anchors.fill: parent
        }
    }

}
