/*
 *  Copyright 2013-2017  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.0 as Kirigami2
import artikulate 1.0

Kirigami2.ApplicationWindow {
    id: root

    function changePage(pageItem) {
        root.pageStack.clear();
        root.pageStack.push(pageItem);
        root.pageStack.push(pageItem);
    }

    globalDrawer: ArtikulateDrawer {
        pageStack: root.pageStack
    }

    contextDrawer: Kirigami2.OverlayDrawer {
        id: contextDrawer
    }

    signal ghnsCourseDataStatusChanged();
    signal triggerSettingsDialog();
    signal triggerAction(string actionName);
    signal switchMenuBarVisibility();

    property Learner learner: g_profileManager.activeProfile

    CourseModel {
        id: availableCourseModel
    }

    pageStack.initialPage: welcomePageComponent
    pageStack.globalToolBar.style: Kirigami2.ApplicationHeaderStyle.Titles

    // pages
    Component {
        id: welcomePageComponent
        WelcomePage { }
    }
    Component {
        id: trainingPageComponent
        TrainingPage { }
    }
    Component {
        id: profileSettingsPageComponent
        ProfileSettingsPage { }
    }
    Component {
        id: downloadPageComponent
        DownloadPage {
            onStatusChanged: root.ghnsCourseDataStatusChanged()
        }
    }
}
