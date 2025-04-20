// SPDX-FileCopyrightText: 2013-2017 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami as Kirigami
import org.kde.artikulate

pragma ComponentBehavior: Bound

Kirigami.ApplicationWindow {
    id: root

    // - layer 0: welcome page
    // - layer 1: training page
    // - layer 2: any additional page
    pageStack.initialPage:[ welcomePageComponent ]
    pageStack.globalToolBar.style: Kirigami.ApplicationHeaderStyle.Titles

    required property var aboutData

    globalDrawer: ArtikulateDrawer {
        width: 300
        pageStack: root.pageStack
        parentWindow: root
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

    CourseModel {
        id: availableCourseModel
    }

    Component {
        id: welcomePageComponent
        WelcomePage { }
    }
    Component {
        id: trainingPageComponent
        TrainingPage { }
    }

    Component {
        id: aboutPageComponent
        Kirigami.AboutPage {
            aboutData: root.aboutData
        }
    }
}
