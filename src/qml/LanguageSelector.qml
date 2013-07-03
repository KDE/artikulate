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
import artikulate 1.0

Item {
    id: root

    property ResourceManager resourceManager
    property Language selectedLanguage

    signal languageSelected(variant language)

    width: languageList.width
    height: languageList.height

    Component {
        id: itemDelegate

        PlasmaComponents.ToolButton {
            text: model.title + " / " + model.i18nTitle
            iconSource: "artikulate-language"
            property Language language: model.dataRole
            onClicked: {
                root.selectedLanguage = language
                languageSelected(language)
            }
        }
    }

    ListView {
        id: languageList

        width: 100
        height: languageList.count * 30
        anchors.fill: parent
        model: LanguageModel { resourceManager: root.resourceManager }
        delegate: itemDelegate
    }
}
