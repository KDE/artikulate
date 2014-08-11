/*
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import org.kde.kquickcontrolsaddons 2.0
import artikulate 1.0

FocusScope {
    id: root

    property bool __showPhonemeUnits: false

    signal languageSelected(variant language)
    signal courseSelected(variant course)
    signal unitSelected(variant unit)

    anchors.fill: parent

    QtObject {
        id: d
    }

    Component.onCompleted: {
        var learner = profileManager.activeProfile;
        if (learner == null) {
            return;
        }
        userProfile.language = resourceManager.language(learner.activeGoal(Learner.Language))
    }

    UnitModel {
        id: selectedUnitModel
        course: userProfile.course
    }

    ToolBar {
        id: header
        width: root.width
        RowLayout {
            ToolButton {
                text: profileManager.activeProfile == null
                    ? i18n("Create Learner Identity")
                    : profileManager.activeProfile.name
                iconName: "user-identity"
                onClicked: {
                    if (profileSelectorSheet.isOpen()) {
                        profileSelectorSheet.close()
                    }
                    else {
                        profileSelectorSheet.open()
                    }
                }
                checked: profileSelectorSheet.isOpen()
            }
        }
    }

    Rectangle {
        id: languageControls
        width: parent.width
        height: Math.ceil(Math.max(languageSwitcher.height, knsDownloadButton.height) + 6)
        anchors {
            top: header.bottom
            topMargin: 2
        }
        color: theme.backgroundColor

        Row {
            spacing: 20
            anchors {
                topMargin: 3
                top: parent.top
                bottomMargin: 3
                bottom: parent.bottom
                leftMargin: 5
                left: parent.left
            }
            LanguageSwitcher {
                id: languageSwitcher
                anchors.verticalCenter: parent.verticalCenter
                width: root.width - 6 - knsDownloadButton.width - 20
                visible: learner != null
                resourceManager: globalResourceManager
                onLanguageSelected: {
                    root.languageSelected(selectedLanguage)
                    if (selectedLanguage != null) {
                        learner.setActiveGoal(Learner.Language, selectedLanguage.id)
                    }
                }
            }
            Row { // show information when no user profile exists
                width: root.width - 6 - knsDownloadButton.width - 20
                visible: learner == null
                spacing: 10
                QIconItem {
                    id: icon
                    icon: "dialog-information"
                    width: theme.mediumIconSize
                    height: theme.mediumIconSize
                    anchors.verticalCenter: parent.verticalCenter
                }
                Label {
                    id: favoritesUnsetInformation
                    anchors.verticalCenter: parent.verticalCenter
                    height: paintedHeight
                    font.pointSize: 1.5 * theme.fontPointSize
                    text: i18n("Start by creating a learner identity")
                }
            }

            ToolButton {
                id: knsDownloadButton
                iconName: "get-hot-new-stuff"
                anchors {
                    verticalCenter: parent.verticalCenter
                }
                enabled: true
                onClicked : {
                    downloadNewStuff()
                }
            }
        }
    }

    Row {
        id: content
        anchors {
            top: languageControls.bottom
            left: languageControls.left
            leftMargin: 10
            topMargin: 10
        }
//         property int columnWidth: Math.floor(root.width / 2 - line.width/2)
        height: root.height - languageControls.height - header.height - 10

        Column {
            width: content.columnWidth
            spacing: 10

            CourseSwitcher {
                id: courseSelector
                resourceManager: globalResourceManager
                view: kcfg_UseContributorResources ? CourseFilterModel.AllResources : CourseFilterModel.OnlyGetHotNewStuffResources
                language: userProfile.language
                onCourseSelected: {
                    userProfile.course = course
                    root.courseSelected(course)
                }
            }

//TODO uncomment for now
// there is currently no course the sufficiently supports this feature
//
//         // additional information for current selector
//         PlasmaComponents.ButtonColumn {
//             visible: userProfile.course != null
//             anchors { leftMargin : 30; left : parent.left }
//             PlasmaComponents.RadioButton {
//                 text: i18n("Scenario Training Units")
//                 checked: !root.__showPhonemeUnits
//                 onClicked : {
//                     root.__showPhonemeUnits = !root.__showPhonemeUnits
//                 }
//             }
//             PlasmaComponents.RadioButton {
//                 text: i18n("Phoneme Training Units")
//                 checked: root.__showPhonemeUnits
//                 onClicked : {
//                     root.__showPhonemeUnits = !root.__showPhonemeUnits
//                 }
//             }
//         }

            ScrollView {
                ListView {
                    id: unitList
                    width: content.columnWidth - 20
                    height: content.height - 60
                    clip: true
                    model: UnitFilterModel {
                        unitModel: selectedUnitModel
                    }
                    delegate: ListItem {
                        width : unitList.width - unitListScrollbar.width - 10
                        title : model.title
                        iconName : "artikulate-course"
                        property Unit unit : model.dataRole
                        onSelected : {
                            unitList.currentIndex = index
                            userProfile.unit = unit
                        }
                    }
                }
            }

            Column {
                visible: userProfile.course != null && userProfile.unit == null && root.__showPhonemeUnits == true
                PhonemeUnitSelector {
                    id: phonemeUnitSelector
                    anchors {
                        leftMargin: 10
                        left: parent.left
                    }
                    width: content.columnWidth
                    course: userProfile.course
                    onUnitSelected: {
                        userProfile.unit = unit //TODO remove after porting to training session
                        trainingSession.createFromUnit(unit);
                        root.unitSelected(unit)
                    }
                }
            }
        }

//         PlasmaCore.SvgItem {
//             id: line
//             width: naturalSize.width
//             height: parent.height
//             elementId: "vertical-line"
//             svg: PlasmaCore.Svg {
//                 imagePath : "widgets/line"
//             }
//         }

        Column {
//             width: Math.floor(root.width / 2 - line.width/2)

            // dummy item
            Item {
                height: Math.floor(content.height * 0.2)
                width: parent.width
            }

            Button {
                id: selectButton
                anchors.horizontalCenter: parent.horizontalCenter
                iconName : "go-next-view"
                text: i18n("Start Training")
                enabled: userProfile.unit != null
                onClicked: {
                    root.unitSelected(userProfile.unit)
                    trainingSession.createFromUnit(userProfile.unit)
                }
            }
        }
    }

    SheetDialog {
        id: profileSelectorSheet
        anchors {
            top: root.top
            topMargin: header.height
            left: root.left
            bottom: root.bottom
            right: root.right
        }
        content: ProfileSelector {
            anchors.fill: parent
            onProfileChosen: {
                profileSelectorSheet.close()
            }
        }
    }
}
