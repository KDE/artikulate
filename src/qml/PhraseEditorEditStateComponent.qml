/*
 *  SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.1
import QtQuick.Controls 1.4
import QtQuick.Layouts 1.2
import artikulate 1.0

Item {
    id: root

    property Phrase phrase

    width: buttons.width
    height: buttons.height

    Component.onCompleted: {
        updateCheckedStates();
    }

    onPhraseChanged: {
        updateCheckedStates()
    }

    function updateCheckedStates()
    {
        if (root.phrase == null) {
            return;
        }
        switch (root.phrase.editState) {
        case Phrase.Unknown:
            buttonUnknown.checked = true;
            break;
        case Phrase.Translated:
            buttonTranslated.checked = true;
            break;
        case Phrase.Completed:
            buttonCompleted.checked = true;
            break;
        }
    }

    GroupBox {
        id: buttons
        title: i18n("Edit State:")
        RowLayout {
            ExclusiveGroup { id: editStateGroup }
            RadioButton {
                id: buttonUnknown
                text: i18nc("state", "Unknown")
                onCheckedChanged: {
                    if (!checked) return
                    root.phrase.editState = Phrase.Unknown
                }
                exclusiveGroup: editStateGroup
            }
            RadioButton {
                id: buttonTranslated
                text: i18nc("state", "Translated")
                onCheckedChanged: {
                    if (!checked) return
                    root.phrase.editState = Phrase.Translated
                }
                exclusiveGroup: editStateGroup
            }
            RadioButton {
                id: buttonCompleted
                text: i18nc("state", "Completed")
                onCheckedChanged: {
                    if (!checked) return
                    root.phrase.editState = Phrase.Completed
                }
                exclusiveGroup: editStateGroup
            }
        }
    }
}
