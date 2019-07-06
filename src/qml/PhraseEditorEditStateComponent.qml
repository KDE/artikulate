/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
