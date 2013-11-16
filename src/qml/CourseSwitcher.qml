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

    property ResourceManager resourceManager
    property Language language
    property Course selectedCourse
    property int view : CourseModel.AllResources

    signal courseSelected(variant course)

    width: 300
    height: Math.max(buttonLeft.height, courseView.height)

    Component {
        id: itemDelegate

        Row {
            width : root.width - buttonLeft.width - buttonRight.width
            height : theme.mediumIconSize
            spacing : 10
            PlasmaCore.IconItem {
                id: icon
                source : "artikulate-course"
                width : theme.smallMediumIconSize
                height : theme.smallMediumIconSize
                anchors.verticalCenter: parent.verticalCenter
            }
            PlasmaComponents.Label {
                id: courseTitleLabel
                anchors.verticalCenter: parent.verticalCenter
                height: paintedHeight
                font.pointSize: theme.defaultFont.pointSize
                text : model.title
            }
        }
    }

    Row {
        spacing : 10
        PlasmaComponents.ToolButton {
            id : buttonLeft
            iconSource : "arrow-left"
            enabled : courseView.currentIndex > 0
            onClicked: {
                languageView.decrementCurrentIndex()
            }
        }

        ListView {
            id: courseView

            width : root.width - buttonLeft.width - buttonRight.width
            height : theme.mediumIconSize

            clip : true
            snapMode : ListView.SnapToItem
            orientation : ListView.Horizontal
            model : CourseModel {
                id : courseModel
                resourceManager : root.resourceManager
                language: root.language
                view: root.view
            }
            onCurrentIndexChanged : {
                selectedCourse = courseModel.course(currentIndex)
                courseSelected(selectedCourse)
            }
            delegate : itemDelegate
        }

        PlasmaComponents.ToolButton {
            id : buttonRight
            enabled : courseView.currentIndex <  courseView.count - 2
            iconSource : "arrow-right"
            onClicked : {
                courseView.incrementCurrentIndex()
            }
        }
    }
}
