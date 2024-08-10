// SPDX-FileCopyrightText: 2013-2017 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.artikulate

Kirigami.ApplicationWindow {
    id: root

    function changePage(pageItem) {
        root.pageStack.clear();
        root.pageStack.push(pageItem);
        root.pageStack.push(pageItem);
    }

    globalDrawer: ArtikulateDrawer {
        width: 300
        pageStack: root.pageStack
    }

    contextDrawer: Kirigami.ContextDrawer {
        id: contextDrawer
        enabled: true
        handleVisible: false
    }

    signal ghnsCourseDataStatusChanged();
    signal triggerSettingsDialog();
    signal triggerAction(string actionName);
    signal switchMenuBarVisibility();

    property Learner learner: g_profileManager.activeProfile

    CourseModel {
        id: availableCourseModel
    }

    pageStack.initialPage:[ welcomePageComponent ]
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
        Kirigami.AboutPage {
            aboutData: g_artikulateAboutData
        }
    }
}
