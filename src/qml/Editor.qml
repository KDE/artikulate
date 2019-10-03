/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

import QtQuick 2.5
import QtQuick.Controls 2.3
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Layouts 1.2
import QtQml.Models 2.2
import org.kde.kirigami 2.7 as Kirigami
import artikulate 1.0

Kirigami.ApplicationWindow {
    id: root

    function changePage(pageItem) {
        root.pageStack.clear();
        root.pageStack.push(pageItem);
        root.pageStack.push(pageItem);
    }

    globalDrawer: EditorDrawer {
        pageStack: root.pageStack
    }

    pageStack.initialPage: welcomePageComponent
    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.Titles

    // pages
    Component {
        id: welcomePageComponent
        WelcomePageEditor { }
    }
    Component {
        id: editCoursePageComponent
        EditCoursePage { }
    }
    Component {
        id: repositoryPageComponent
        Item {
            // TODO not implemented yet
        }
    }
    Component {
        id: aboutPageComponent
        Kirigami.AboutPage {
            aboutData: g_artikulateAboutData
        }
    }
}
