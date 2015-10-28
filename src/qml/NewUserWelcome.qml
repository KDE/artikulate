/*
 *  Copyright 2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import QtQuick.Layouts 1.2
import artikulate 1.0

Item {
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: true
    height: 50

    ColumnLayout {
        id: row

        Text {
            text: i18n("Welcome to Artikulate!")
            font.pointSize: 24
        }

        Text {
            text: i18n("There are no courses installed on this system.")
        }
        Text {
            text: i18n("You can install them now from the Internet:")
        }

        Button {
            text: i18n("Download Courses")
            iconName: "get-hot-new-stuff"
            onClicked: triggerDownloadCourses()
        }
    }
}

