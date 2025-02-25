// SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import org.kde.kirigami as Kirigami
import org.kde.kirigamiaddons.formcard as FormCard
import org.kde.artikulate

Kirigami.ApplicationWindow {
    id: root

    function changePage(pageItem) {
        root.pageStack.clear();
        root.pageStack.push(pageItem);
    }

    globalDrawer: EditorDrawer {
        width: 300
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
        FormCard.AboutPage {
            aboutData: g_artikulateAboutData
        }
    }

    Component.onCompleted: EditorSession.repository = g_repository
}
