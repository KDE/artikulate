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
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2
import artikulate 1.0

Item {
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: true
    height: 50

    property Unit unit
    property bool editPhrases : false

    ColumnLayout {
        id: row

        RowLayout {
            Text {
                text: i18n("Unit Title:")
            }
            TextField {
                width: 300
                text: unit.title
                onEditingFinished: {
                    unit.title = text
                }
            }
        }
        Button { // add units only if skeleton
            id: newUnitButton
            visible: root.editPhrases
            iconName: "list-add"
            text: i18n("Create Phrase")
            onClicked: phraseModel.course.createPhrase(unit)
        }
        Item { //dummy
            Layout.fillHeight: true
        }
    }
}

