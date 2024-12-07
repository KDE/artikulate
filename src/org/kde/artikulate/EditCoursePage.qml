// SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQml.Models
import org.kde.kirigami as Kirigami
import org.kde.artikulate

Kirigami.ScrollablePage {
    id: root
    title: i18n("Edit Course")

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
        Kirigami.Theme.colorSet: Kirigami.Theme.View
    }

    actions: [
        Kirigami.Action {
            text: i18n("Previous")
            tooltip: i18n("Switch to previous phrase.")
            icon.name: "go-previous"
            enabled: g_editorSession.hasPreviousPhrase
            onTriggered: g_editorSession.switchToPreviousPhrase()
        },
        Kirigami.Action {
            text: i18n("Next")
            tooltip: i18n("Switch to next phrase.")
            icon.name: "go-next"
            enabled: g_editorSession.hasNextPhrase
            onTriggered: g_editorSession.switchToNextPhrase()
        },
        Kirigami.Action {
            separator: true
        },
        Kirigami.Action {
            text: i18n("Delete")
            tooltip: i18n("Delete this phrase.")
            icon.name: "edit-delete-remove"
            onTriggered: g_editorSession.course.deletePhrase(g_editorSession.phrase)
        },
        Kirigami.Action {
            text: i18n("Create Phrase")
            tooltip: i18n("Create phrase after current phrase.")
            icon.name: "list-add"
            onTriggered: g_editorSession.course.createPhraseAfter(g_editorSession.phrase)
        }
    ]

    ColumnLayout {
        PhraseEditor {
            visible: g_editorSession.phrase !== null
            phrase: g_editorSession.phrase
            isSkeletonPhrase: g_editorSession.skeletonMode
            Layout.fillHeight: true
        }
    }
}
