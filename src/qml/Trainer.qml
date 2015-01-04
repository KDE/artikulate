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

    property ResourceManager resourceManager: globalResourceManager

    Component.onCompleted: {
        var learner = profileManager.activeProfile;
        if (learner == null) {
            return;
        }
        trainingSession2.language = globalResourceManager.language(learner.activeGoal(Learner.Language))
    }

    CourseModel {
        id: availableCourseModel
        resourceManager: globalResourceManager
    }

    UnitModel {
        id: selectedUnitModel
        course: trainingSession2.course
    }

    ColumnLayout {
        RowLayout {
            spacing: 6
            LanguageSwitcher {
                id: languageSwitcher
                width: 400
                anchors.verticalCenter: parent.verticalCenter
                visible: learner != null
                resourceManager: globalResourceManager
                onLanguageSelected: {
                    trainingSession2.language = selectedLanguage
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
        }

        RowLayout {
            ColumnLayout {
                ScrollView {
                    Layout.minimumWidth: 300
                    Layout.minimumHeight: 700
                    ListView {
                        id: unitList
                        clip: true
                        model: UnitFilterModel {
                            unitModel: selectedUnitModel
                        }
                        delegate: ListItem {
                            width : unitList.width - 10 - 10
                            title : model.title
                            iconName : "artikulate-course"
                            property Unit unit : model.dataRole
                            onSelected : {
                                unitList.currentIndex = index
                                trainingSession2.unit = unit
                                trainingSession.createFromUnit(trainingSession2.unit)
                            }
                        }
                    }
                }
            }
            ColumnLayout {
                CourseSwitcher {
                    id: courseSelector
                    resourceManager: globalResourceManager
                    view: kcfg_UseContributorResources ? CourseFilterModel.AllResources : CourseFilterModel.OnlyGetHotNewStuffResources
                    language: trainingSession2.language
                    onCourseSelected: {
                        trainingSession2.course = course
                    }
                }

                TrainerCourse {
                    width: 800
                    height: 600
                    session: trainingSession
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
