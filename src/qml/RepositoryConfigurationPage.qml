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
