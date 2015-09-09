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
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.3
import artikulate 1.0

Item {
    id: root

    anchors {
        fill: parent
        topMargin: 6
        rightMargin: 6
        bottomMargin: 6
        leftMargin: 6
    }

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
        id: content
        RowLayout {
            spacing: 6
            LanguageSwitcher {
                id: languageSwitcher
                width: 400
                anchors.verticalCenter: parent.verticalCenter
                visible: learner != null
                resourceManager: g_resourceManager
                onLanguageSelected: {
                    g_trainingSession.language = selectedLanguage
                }
            }

            ToolButton {
                id: knsDownloadButton
                iconName: "get-hot-new-stuff"
                anchors {
                    verticalCenter: parent.verticalCenter
                }
                enabled: true
                onClicked: downloadNewStuff()
            }
            Text{ text: content.height}
        }

        RowLayout {
            ScrollView {
                Layout.fillHeight: true
                Layout.preferredWidth: 300
                Layout.preferredHeight: content.height - languageSwitcher.height - 12
                ListView {
                    id: unitList
                    clip: true
                    model: UnitFilterModel {
                        unitModel: UnitModel { course: g_trainingSession.course }
                    }
                    delegate: ListItem {
                        width : unitList.width - 10 - 10
                        title : model.title
                        iconName : "artikulate-course"
                        property Unit unit : model.dataRole
                        onSelected : {
                            unitList.currentIndex = index
                            g_trainingSession.unit = unit
                            trainingSession.createFromUnit(g_trainingSession.unit)
                        }
                    }
                }
            }
            ColumnLayout {
                CourseSwitcher {
                    id: courseSelector
                    resourceManager: g_resourceManager
                    view: kcfg_UseContributorResources ? CourseFilterModel.AllResources : CourseFilterModel.OnlyGetHotNewStuffResources
                    language: g_trainingSession.language
                    onCourseSelected: {
                        g_trainingSession.course = course
                    }
                }

                TrainerSessionScreen {
                    id: trainerMain
                    width: 800 //root.width - 40
                    height: 800//root.height - 30
                    unit: g_trainingSession.unit
                    session: trainingSession
                }

                Column {
                    id: progressBar

                    spacing: 10
                    Text {
                        id: trainingText
                        text: i18n("Training Progress")
                        font.pointSize: theme.fontPointSize
                    }
                    TrainerProgressBar {
                        width: trainerMain.width
                        session: trainingSession
                        onTypeSelected: {
                            g_trainingSession.phraseType = type
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: curtain
        anchors.fill: parent
        color: "#000"
        opacity: 0
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
