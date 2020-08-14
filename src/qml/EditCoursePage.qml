/*
 *  SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.5
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.2
import QtQml.Models 2.2
import org.kde.kirigami 2.7 as Kirigami
import artikulate 1.0

Kirigami.Page {
    id: root
    title: i18n("Edit Course")

    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
        Kirigami.Theme.colorSet: Kirigami.Theme.View
    }

    actions {
        left: Kirigami.Action {
            text: i18n("Previous")
            tooltip: i18n("Switch to previous phrase.")
            iconName: "go-previous"
            enabled: g_editorSession.hasPreviousPhrase
            onTriggered: g_editorSession.switchToPreviousPhrase()
        }
        main: Kirigami.Action {
            text: i18n("Next")
            tooltip: i18n("Switch to next phrase.")
            iconName: "go-next"
            enabled: g_editorSession.hasNextPhrase
            onTriggered: g_editorSession.switchToNextPhrase()
        }
        contextualActions: [
            Kirigami.Action {
                text: i18n("Delete")
                tooltip: i18n("Delete this phrase.")
                iconName: "edit-delete-remove"
                onTriggered: g_editorSession.course.deletePhrase(g_editorSession.phrase)
            },
            Kirigami.Action {
                separator: true
            },
            Kirigami.Action {
                text: i18n("Create Phrase")
                tooltip: i18n("Create phrase after current phrase.")
                iconName: "list-add"
                onTriggered: g_editorSession.course.createPhraseAfter(g_editorSession.phrase)
            }
        ]
    }

    ColumnLayout {
        PhraseEditor {
            visible: g_editorSession.phrase !== null
            phrase: g_editorSession.phrase
            isSkeletonPhrase: g_editorSession.skeletonMode
            Layout.fillHeight: true
        }
    }
}
