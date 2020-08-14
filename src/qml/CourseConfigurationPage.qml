/*
 *  SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.4
import QtQuick.Dialogs 1.2
import org.kde.kirigami 2.7 as Kirigami

Kirigami.Page {
    id: root
    title: g_editorSession.skeletonMode ? i18n("Prototype Configuration") : i18n("Course Configuration")

    Kirigami.FormLayout {
        anchors.fill: parent
        Kirigami.Separator {
            Kirigami.FormData.label: g_editorSession.skeletonMode ? i18n("Prototype Description") : i18n("Course Description")
            Kirigami.FormData.isSection: true
        }
        TextField {
            Kirigami.FormData.label: i18n("Title:")
            text: g_editorSession.course.title
            Layout.preferredWidth: .7 * root.width
            onAccepted: g_editorSession.course.title = text
        }
        TextField {
            visible: !g_editorSession.skeletonMode
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
            visible: !g_editorSession.skeletonMode
            Kirigami.FormData.label: i18n("Language:")
            text: g_editorSession.course.languageTitle
            readOnly: true
            Layout.preferredWidth: .7 * root.width
        }

        Kirigami.Separator {
            visible: !g_editorSession.skeletonMode
            Kirigami.FormData.label: i18n("Prototype")
            Kirigami.FormData.isSection: true
        }
        Button {
            visible: !g_editorSession.skeletonMode
            Kirigami.FormData.label: i18n("Update from Prototype:")
            enabled: g_editorSession.skeletonMode
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
