/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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

    width: courseList.width
    height: courseList.count > 0 ? courseList.height : noCoursesMessage.height

    Component {
        id: courseDelegate

        PlasmaComponents.ToolButton {
            property Course course: model.dataRole

            text : model.title
            onClicked: {
                root.selectedCourse = course
            }
        }
    }

    ListView {
        id: courseList

        width: 100
        height: 30 * courseList.count
        visible: courseList.count > 0
        anchors.fill: parent
        model: CourseModel {
            resourceManager: root.resourceManager
            language: root.language
        }
        delegate: courseDelegate
    }

    Text {
        id: noCoursesMessage
        property string message

        visible: courseList.count == 0
        text: message
        onVisibleChanged: {
            if (root.courseModel == null) {
                return;
            }
            if (root.courseModel.language == null) {
                message = i18n("Please select a language.");
            }
            else {
                message = i18n("There are no courses for the selected language.");
            }
        }
    }
}
