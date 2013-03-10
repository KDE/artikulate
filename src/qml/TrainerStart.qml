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

FocusScope {
    id: screen
    height: screen.height
    width: screen.width

    property LanguageModel languageModel
    property CourseModel courseModel
    signal languageSelected(variant language)
    signal courseSelected(variant course)
    signal unitSelected(variant unit)

    QtObject {
        id: d
    }

    function start() {}
    function reset() {
        //TODO
    }

    Column {
        id: helloArtikulate
        anchors.top: screen.top
        anchors.left: screen.left
        anchors.topMargin: 30
        anchors.leftMargin: 30
        spacing: 10

        Text {
            text: i18n("Welcome to")
            font.pointSize: 28;
        }
        Text {
            text: i18n("Artikulate!")
            font.pointSize: 48;
        }
    }

    Item {
        id: breadcrumb
        anchors.top: helloArtikulate.bottom
        anchors.left: helloArtikulate.left
        anchors.topMargin: 20
        width: parent. width
        height: 150

        Column {
            Image {
                width: 128
                height: 128
                source: userProfile.language == null ? "../images/language-gray.png" : "../images/language.png"
            }
            PlasmaComponents.ToolButton {
                iconSource: "dialog-close"
                text: userProfile.language != null ? userProfile.language.title : i18n("No language selected")
                flat: true
                enabled: userProfile.language != null
                onClicked : {
                    userProfile.language = null
                    userProfile.course = null
                    userProfile.unit = null
                }
            }
        }

        Column {
            x: parent.width / 3
            Image {
                width: 128
                height: 128
                source: userProfile.course == null ? "../images/course-gray.png" : "../images/course.png"
            }
            PlasmaComponents.ToolButton {
                iconSource: "dialog-close"
                text: userProfile.course != null ? userProfile.course.title : i18n("No course selected")
                flat: true
                enabled: userProfile.course != null
                onClicked : {
                    userProfile.course = null
                    userProfile.unit = null
                }
            }
        }
    }

    Text {
        id: selectNextTipp
        anchors.top: screen.verticalCenter
        anchors.left: helloArtikulate.left
        anchors.topMargin: 20

        text: {
            if (userProfile.language == null) {
                return i18n("Select a language:");
            }
            if (userProfile.language != null && userProfile.course == null) {
                return i18n("Select a course:");
            }
            if (userProfile.course != null && userProfile.unit == null) {
                return i18n("Select a unit:");
            }
        }
        font.pointSize: 24;
    }

    Column {
        spacing: 30
        anchors.top: selectNextTipp.top
        anchors.left: selectNextTipp.right
        anchors.leftMargin: 30
        anchors.topMargin: 10

        LanguageSelector {
            id: languageSelector
            visible: userProfile.language == null
            languageModel: screen.languageModel
            onLanguageSelected: {
                screen.languageSelected(language)
            }
        }

        CourseSelector {
            id: courseSelector
            visible: userProfile.language != null && userProfile.course == null
            courseModel: screen.courseModel
            onCourseSelected: {
                screen.courseSelected(course)
            }
        }
    }
}
