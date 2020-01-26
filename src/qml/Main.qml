/*
 *  Copyright 2013-2017  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.5
import QtQuick.Controls 2.0 as QQC2
import org.kde.kirigami 2.7
import artikulate 1.0

ApplicationWindow {
    id: root

    function changePage(pageItem) {
        root.pageStack.clear();
        root.pageStack.push(pageItem);
        root.pageStack.push(pageItem);
    }

    globalDrawer: ArtikulateDrawer {
        pageStack: root.pageStack
    }

    contextDrawer: OverlayDrawer {
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
    pageStack.globalToolBar.style: ApplicationHeaderStyle.Titles

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
        id: aboutPageComponent
        AboutPage {
            aboutData: g_artikulateAboutData
        }
    }

    Component {
        id: downloadPageComponent
        DownloadPage {
            onStatusChanged: root.ghnsCourseDataStatusChanged()
        }
    }
}
