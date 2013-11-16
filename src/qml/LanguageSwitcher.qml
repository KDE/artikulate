/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
    property int view : LanguageModel.AllLanguages

    signal languageSelected(variant language)

    width: 300
    height: Math.max(buttonLeft.height, languageView.height)

    Component {
        id: itemDelegate

        Row {
            width : root.width - buttonLeft.width - buttonRight.width - 20
            height : theme.mediumIconSize
            spacing : 10
            PlasmaCore.IconItem {
                id: icon
                source : "artikulate-language"
                width : theme.mediumIconSize
                height : theme.mediumIconSize
                anchors.verticalCenter: parent.verticalCenter
            }
            PlasmaComponents.Label {
                id: languageTitleLabel
                anchors.verticalCenter: parent.verticalCenter
                height: paintedHeight
                font.pointSize: 1.5 * theme.defaultFont.pointSize
                text : model.title + " / " + model.i18nTitle
            }
        }
    }

    Row {
        spacing : 10

        ListView {
            id: languageView

            width : root.width - buttonLeft.width - buttonRight.width - 20
            height : theme.mediumIconSize

            clip : true
            snapMode : ListView.SnapToItem
            orientation : ListView.Vertical
            model: LanguageModel {
                id : languageModel
                view: root.view
                resourceModel : LanguageResourceModel { resourceManager: root.resourceManager }
            }
            onCurrentIndexChanged : {
                selectedLanguage = languageModel.language(currentIndex)
                languageSelected(selectedLanguage)
            }
            delegate : itemDelegate
        }

        PlasmaComponents.ToolButton {
            id : buttonLeft
            iconSource : "arrow-left"
            enabled : languageView.currentIndex > 0
            onClicked: {
                languageView.decrementCurrentIndex()
            }
        }

        PlasmaComponents.ToolButton {
            id : buttonRight
            enabled : languageView.currentIndex <  languageView.count - 2
            iconSource : "arrow-right"
            onClicked : {
                languageView.incrementCurrentIndex()
            }
        }
    }
}
