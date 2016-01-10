/*
 *  Copyright 2012       Sebastian Gottfried <sebastiangottfried@web.de>
 *  Copyright 2013-2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
    anchors.fill: parent

    property ProfileManager profileManager: null

    ScrollView {
        anchors {
            fill: parent
            topMargin: 5
            rightMargin: 5
            bottomMargin: 5
            leftMargin: 5
        }
        ListView {
            id: goalList
            width: root.width
            clip: true
            model: LearningGoalModel {
                profileManager: root.profileManager
                learner: profileManager.activeProfile
            }
            delegate: ListItem {
                id: goalSelector
                property LearningGoal goal: model.dataRole
                width: goalList.width - 10
                title: model.title
                iconName: "favorites"
                onSelected: goalList.currentIndex = index
            }
        }
    }
}
