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
import org.kde.kquickcontrolsaddons 2.0
import artikulate 1.0

Item {
    id: root

    property ResourceManager resourceManager
    property Language language
    property Course selectedCourse
    property int view : CourseFilterModel.AllResources

    signal courseSelected(variant course)

    width: 300
    height: Math.max(buttonLeft.height, courseView.height)

    onLanguageChanged: {
        if (courseModel.course(courseView.currentIndex) != null) {
            selectedCourse = courseModel.course(courseView.currentIndex)
            courseSelected(selectedCourse)
        }
    }

    Component.onCompleted: {
        courseView.currentIndex = 0
    }

    Component {
        id: itemDelegate

        Row {
            width: root.width - buttonLeft.width - buttonRight.width
            height: theme.mediumIconSize
            spacing: 10
            QIconItem {
                id: icon
                icon: "artikulate-course"
                width: theme.smallMediumIconSize
                height: theme.smallMediumIconSize
                anchors.verticalCenter: parent.verticalCenter
            }
            Label {
                id: courseTitleLabel
                anchors.verticalCenter: parent.verticalCenter
                height: paintedHeight
                font.pointSize: theme.fontPointSize
                text: model.title
            }
        }
    }

    Row {
        spacing : 10
        ToolButton {
            id : buttonLeft
            iconName: "arrow-left"
            enabled: courseView.currentIndex > 0
            onClicked: {
                languageView.decrementCurrentIndex()
            }
        }

        ListView {
            id: courseView

            width: root.width - buttonLeft.width - buttonRight.width
            height: theme.mediumIconSize

            clip: true
            snapMode: ListView.SnapToItem
            orientation: ListView.Horizontal
            model: CourseFilterModel {
                view: root.view
                courseModel: CourseModel {
                    id: courseModel
                    resourceManager : root.resourceManager
                    language: root.language
                }
            }
            onCurrentIndexChanged: {
                if (courseModel.language == null) {
                    return;
                }
                if (courseModel.course(currentIndex) != null) {
                    selectedCourse = courseModel.course(currentIndex)
                    courseSelected(selectedCourse)
                }
            }
            delegate : itemDelegate
        }

        ToolButton {
            id: buttonRight
            enabled: courseView.currentIndex <  courseView.count - 2
            iconName: "arrow-right"
            onClicked: {
                courseView.incrementCurrentIndex()
            }
        }
    }
}
