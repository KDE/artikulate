// SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Dialogs
import org.kde.kirigami as Kirigami

Kirigami.Page {
    id: root

    title: i18n("Repository Configuration")

    FileDialog {
        id: fileDialog
        modality: Qt.WindowModal
        title: i18n("Select Repository Folder")
        selectFolder: true
        folder: g_repository.repositoryUrl
        sidebarVisible: true
        onAccepted: {
            console.log("Accepted: " + fileUrl)
            g_repository.repositoryUrl = fileUrl
        }
    }

    Kirigami.FormLayout {
        anchors.fill: parent
        TextField {
            Kirigami.FormData.label: i18n("Repository:")
            readOnly: true
            text: g_repository.repositoryUrl.toString().replace(/^(file:\/{2})/,"");
        }
        Button {
            text: i18n("Change Folder")
            icon.name: "document-open-folder"
            onClicked: fileDialog.open()
        }
    }
}
