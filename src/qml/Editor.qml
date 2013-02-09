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

import QtQuick 1.0
import artikulate 1.0
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents

Item
{
    id: editor

    ApplicationBackground {
        id: background
        anchors.fill: parent
    }

    LanguageModel {
        id: availableLanguageModel
        resourceManager: globalResourceManager
    }

    CourseModel {
        id: availableCourseModel
        resourceManager: globalResourceManager
    }

    Column {
        Text {
            text: i18n("<h1>Course Editor</h1>")
        }

        Row {
            spacing: 20
            anchors.horizontalCenter: parent.horizontalCenter

            Column {
                Text { text: i18n("<h2>Languages</h2>") }
                LanguageSelector {
                    id: languageSelector
                    languageModel: availableLanguageModel
                    onLanguageSelected: {
                        availableCourseModel.language = language
                   }
                }
            }

            Column {
                Text { text: i18n("<h2>Courses</h2>") }
                PlasmaComponents.ToolButton {
                    text: i18n("New Course")
                    iconSource: "document-new"
                    enabled: {
                        availableCourseModel.language != null
                    }
                    onClicked: {
                        globalResourceManager.newCourseDialog();
                    }
                }
                CourseSelector {
                    id: courseSelector
                    courseModel: availableCourseModel
                    onCourseSelected: {
                        //TODO select unit
                    }
                }
            }
        }
    }
}
