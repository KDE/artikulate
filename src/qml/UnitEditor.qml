/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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

import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.extras 0.1 as PlasmaExtras
import artikulate 1.0

Item {
    id: root

    property Unit unit

    width: 500
    height: 400

    PhraseModel {
        id: phraseModel
        unit: root.unit
    }

    Component {
        id: itemDelegate

        PhraseEditor {
            phrase: model.dataRole
        }
    }

    PlasmaComponents.ToolButton {
        id: addPhraseButton
        text: i18n("Add Phrase")
        iconSource: "document-new"
        enabled: {
            root.unit != null
        }
        onClicked: {
            root.unit.course.createPhrase(unit)
        }
    }
    Item {
        anchors.top: addPhraseButton.bottom
        width: root.width
        height: root.height
        PlasmaExtras.ScrollArea {
            anchors.fill: parent

            ListView {
                anchors.fill: parent
                model: phraseModel
                delegate: itemDelegate
            }
        }
    }
}
