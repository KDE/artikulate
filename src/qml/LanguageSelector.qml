/*
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
import artikulate 1.0

Item {
    id: root

    property ResourceManager resourceManager
    property Language selectedLanguage
    property int view: LanguageModel.AllLanguages

    signal languageSelected(variant language)

    width: 300
    height: 300

    Component {
        id: itemDelegate

        ToolButton {
            id: button

            width: root.width - 20
            text: model.title + " / " + model.i18nTitle
            iconName: "artikulate-language"
            property Language language: model.dataRole
            onClicked: {
                root.selectedLanguage = language
                languageSelected(language)
            }
        }
    }

    ScrollView {
        ListView {
            id: languageList

            anchors.fill: parent
            clip: true
            model: LanguageModel {
                view: root.view
                resourceModel: LanguageResourceModel {resourceManager: root.resourceManager }
            }
            delegate: itemDelegate
        }
    }
}
