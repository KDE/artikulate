/*
    SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

import QtQuick 2.5
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import org.kde.kirigami 2.7 as Kirigami
import artikulate 1.0

Kirigami.ApplicationWindow {
    id: root

    function changePage(pageItem) {
        root.pageStack.clear();
        root.pageStack.push(pageItem);
    }

    globalDrawer: EditorDrawer {
        pageStack: root.pageStack
    }

    contextDrawer: Kirigami.ContextDrawer {
        id: mainFlickable //ID is workaround for Kirigami's assumption that this is the name of the drawer's main flickable
    }

    pageStack.initialPage: [ editorCourseSelectionPage, editorSkeletonSelectionPage ]
    pageStack.globalToolBar.style: pageStack.depth > 1 ? Kirigami.ApplicationHeaderStyle.TabBar :Kirigami.ApplicationHeaderStyle.Titles

    // pages
    Component {
        id: editorCourseSelectionPage
        EditorCourseSelectionPage { }
    }
    Component {
        id: editorSkeletonSelectionPage
        EditorSkeletonSelectionPage { }
    }
    Component {
        id: editCoursePageComponent
        EditCoursePage { }
    }
    Component {
        id: courseConfigurationPageComponent
        CourseConfigurationPage { }
    }
    Component {
        id: repositoryPageComponent
        RepositoryConfigurationPage { }
    }
    Component {
        id: aboutPageComponent
        Kirigami.AboutPage {
            aboutData: g_artikulateAboutData
        }
    }
}
