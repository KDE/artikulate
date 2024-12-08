// SPDX-FileCopyrightText: 2019-2024 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami

Kirigami.Page {
    id: root
    title: EditorSession.skeletonMode ? i18n("Prototype Configuration") : i18n("Course Configuration")

    Kirigami.FormLayout {
        anchors.fill: parent
        Kirigami.Separator {
            Kirigami.FormData.label: EditorSession.skeletonMode ? i18n("Prototype Description") : i18n("Course Description")
            Kirigami.FormData.isSection: true
        }
        TextField {
            Kirigami.FormData.label: i18n("Title:")
            text: EditorSession.course.title
            Layout.preferredWidth: .7 * root.width
            onAccepted: EditorSession.course.title = text
        }
        TextField {
            visible: !EditorSession.skeletonMode
            Kirigami.FormData.label: i18n("Localized Title:")
            text: EditorSession.course.i18nTitle
            Layout.preferredWidth: .7 * root.width
            onAccepted: EditorSession.course.i18nTitle = text
        }
        TextField {
            Kirigami.FormData.label: i18n("Description:")
            text: EditorSession.course.description
            Layout.preferredWidth: .7 * root.width
            onAccepted: EditorSession.course.description = text
        }
        TextField {
            visible: !EditorSession.skeletonMode
            Kirigami.FormData.label: i18n("Language:")
            text: EditorSession.course.languageTitle
            readOnly: true
            Layout.preferredWidth: .7 * root.width
        }

        Kirigami.Separator {
            visible: !EditorSession.skeletonMode
            Kirigami.FormData.label: i18n("Prototype")
            Kirigami.FormData.isSection: true
        }
        Button {
            visible: !EditorSession.skeletonMode
            Kirigami.FormData.label: i18n("Update from Prototype:")
            enabled: EditorSession.skeletonMode
            Layout.minimumWidth: 200
            text: i18n("Update")
            icon.name: "view-refresh"
            ToolTip.visible: hovered
            ToolTip.delay: 1000
            ToolTip.timeout: 5000
            ToolTip.text: i18n("Update the course with elements from prototype.")
            onClicked: EditorSession.updateCourseFromSkeleton()
        }

// TODO add export functionalities
//        Kirigami.Separator {
//            Kirigami.FormData.label: i18n("Export")
//            Kirigami.FormData.isSection: true
//        }

    }
}
