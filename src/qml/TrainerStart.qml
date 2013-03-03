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
        spacing: 30
        height: screen.height
        width: screen.width

        Text {
            text: i18n("<h1>Artikulate</h1>")
            anchors.horizontalCenter: parent.horizontalCenter
        }
        Row {
            spacing: 20
            anchors.horizontalCenter: parent.horizontalCenter

            Column {
                Text { text: i18n("<h2>Languages</h2>") }
                LanguageSelector {
                    id: languageSelector
                    languageModel: screen.languageModel
                    onLanguageSelected: {
                        screen.languageSelected(language)
                    }
                }
            }

            Column {
                Text { text: i18n("<h2>Courses</h2>") }
                CourseSelector {
                    id: courseSelector
                    courseModel: screen.courseModel
                    onCourseSelected: {
                        screen.courseSelected(course)
                    }
                }
            }
        }
    }
}