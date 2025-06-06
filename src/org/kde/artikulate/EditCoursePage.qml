// SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
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
            enabled: EditorSession.hasPreviousPhrase
            onTriggered: EditorSession.switchToPreviousPhrase()
        },
        Kirigami.Action {
            text: i18n("Next")
            tooltip: i18n("Switch to next phrase.")
            icon.name: "go-next"
            enabled: EditorSession.hasNextPhrase
            onTriggered: EditorSession.switchToNextPhrase()
        },
        Kirigami.Action {
            separator: true
        },
        Kirigami.Action {
            text: i18n("Delete")
            tooltip: i18n("Delete this phrase.")
            icon.name: "edit-delete-remove"
            onTriggered: EditorSession.course.deletePhrase(EditorSession.phrase)
        },
        Kirigami.Action {
            text: i18n("Create Phrase")
            tooltip: i18n("Create phrase after current phrase.")
            icon.name: "list-add"
            onTriggered: EditorSession.course.createPhraseAfter(EditorSession.phrase)
        }
    ]

    ColumnLayout {
        PhraseEditor {
            visible: EditorSession.phrase !== null
            phrase: EditorSession.phrase
            isSkeletonPhrase: EditorSession.skeletonMode
            Layout.fillHeight: true
        }
    }
}
