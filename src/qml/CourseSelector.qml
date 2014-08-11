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

import QtQuick 2.1
import QtQuick.Controls 1.2
import artikulate 1.0

Item {
    id: root

    property ResourceManager resourceManager
    property Language language
    property Course selectedCourse
    property int view : CourseFilterModel.AllResources

    signal courseSelected(variant course)

    width: courseList.width
    height: courseList.count > 0 ? courseList.height : noCoursesMessage.height

    Component {
        id: courseDelegate

        ToolButton {
            property Course course: model.dataRole
            iconName: "artikulate-course"
            text: model.title
            onClicked: {
                root.selectedCourse = course
                courseSelected(course)
            }
        }
    }

    ListView {
        id: courseList

        width: 100
        height: 30 * courseList.count
        visible: courseList.count > 0
        anchors.fill: parent
        model: CourseFilterModel {
            view: root.view
            courseModel: CourseModel {
                resourceManager: root.resourceManager
                language: root.language
            }
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
