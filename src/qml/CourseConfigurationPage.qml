/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.2
import org.kde.kirigami 2.7 as Kirigami

Kirigami.Page {
    id: root
    //TODO adapt to editor session if course or skeleton: requires refactoring of editorsession to only operate on one course
    title: i18n("Course Configuration")

    Kirigami.FormLayout {
        anchors.fill: parent
        Kirigami.Separator {
            Kirigami.FormData.label: i18n("Course Description")
            Kirigami.FormData.isSection: true
        }
        TextField {
            Kirigami.FormData.label: i18n("Title:")
            text: g_editorSession.course.title
            Layout.preferredWidth: .7 * root.width
            onAccepted: g_editorSession.course.title = text
        }
        TextField {
            Kirigami.FormData.label: i18n("Localized Title:")
            text: g_editorSession.course.i18nTitle
            Layout.preferredWidth: .7 * root.width
            onAccepted: g_editorSession.course.i18nTitle = text
        }
        TextField {
            Kirigami.FormData.label: i18n("Description:")
            text: g_editorSession.course.description
            Layout.preferredWidth: .7 * root.width
            onAccepted: g_editorSession.course.description = text
        }
        TextField {
            Kirigami.FormData.label: i18n("Language:")
            text: g_editorSession.course.languageTitle
            readOnly: true
            Layout.preferredWidth: .7 * root.width
        }

        Kirigami.Separator {
            Kirigami.FormData.label: i18n("Prototype")
            Kirigami.FormData.isSection: true
        }
        Button {
            Kirigami.FormData.label: i18n("Update from Prototype:")
            enabled: g_editorSession.isSkeletonMode
            Layout.minimumWidth: 200
            text: i18n("Update")
            icon.name: "view-refresh"
            ToolTip.visible: hovered
            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.text: i18n("Update the course with elements from prototype.")
            onClicked: g_editorSession.updateCourseFromSkeleton()
        }

// TODO add export functionalities
//        Kirigami.Separator {
//            Kirigami.FormData.label: i18n("Export")
//            Kirigami.FormData.isSection: true
//        }

    }
}
