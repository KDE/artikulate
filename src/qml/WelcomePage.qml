/*
 *  Copyright 2015-2017  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import QtQuick.Controls 2.1 as QQC2
import org.kde.kirigami 2.0 as Kirigami2
import artikulate 1.0

Kirigami2.Page {
    id: root
    title: i18n("Welcome to Artikulate")

    Column {
        spacing: 20

        Row {
            spacing: 20

            Icon {
                id: langIcon
                icon: "language-artikulate"
                width: 48
                height: 48
            }

            QQC2.ComboBox {
                id: comboLanguage
                width: 200
                model: LanguageModel {
                    id: languageModel
                    resourceModel: LanguageResourceModel {
                        resourceManager: g_resourceManager
                    }
                }
                textRole: "title"
                onCurrentIndexChanged: {
                    if (languageModel.language(currentIndex)) {
                        g_trainingSession.language = languageModel.language(currentIndex)
                    }
                }
            }

            QQC2.ComboBox {
                id: comboCourse
                enabled: {
                    courseFilterModel.filteredCount == 0 ? false : true
                }
                width: 200
                model: CourseFilterModel {
                    id: courseFilterModel
                    view: {
                        kcfg_UseContributorResources
                            ? CourseFilterModel.AllResources
                            : CourseFilterModel.OnlyGetHotNewStuffResources
                    }
                    courseModel: CourseModel {
                        id: courseModel
                        resourceManager: g_resourceManager
                        language: g_trainingSession.language
                        onLanguageChanged: {
                            if (courseFilterModel.course(0)) {
                                g_trainingSession.course = courseFilterModel.course(0)
                            }
                        }
                    }
                }
                textRole: "title"
                onCurrentIndexChanged: {
                    if (courseFilterModel.course(currentIndex)) {
                        g_trainingSession.course = courseFilterModel.course(currentIndex)
                    }
                }
            }
        }

        QQC2.Button {
            text: i18n("Get More Training Material")
            onClicked: triggerDownloadCourses()
        }
    }
}
