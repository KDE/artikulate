/*
 *  SPDX-FileCopyrightText: 2013-2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.1
import QtQuick.Controls 2.3
import artikulate 1.0

Item {
    id: root

    property Course course
    property PhonemeGroup currentPhonemeGroup

    signal unitSelected(variant unit)

    width: phonemeGroupList.width
    height: phonemeGroupList.height

    PhonemeGroupModel {
        id: phonemeGroupModel
        course: root.course
    }

    PhonemeUnitModel {
        id: phonemeUnitModel
        course: root.course
        phonemeGroup: root.currentPhonemeGroup
    }

    onCourseChanged: {
        root.currentPhonemeGroup = null
    }

    Component {
        id: groupDelegate

        Column {
            id: content

            property PhonemeGroup phonemeGroup: model.dataRole

            height: groupSelectButton.height + unitList.height
            width: 200

            ToolButton {
                id: groupSelectButton
                text : model.title

                onClicked: {
                    root.currentPhonemeGroup = phonemeGroup
                }
            }

            ListView {
                id: unitList
                visible: root.currentPhonemeGroup == phonemeGroup
                height: unitList.visible ? 30 * phonemeUnitModel.count : 0
                width: 200

                model: phonemeUnitModel
                delegate: unitDelegate
            }
        }
    }

    Component {
        id: unitDelegate

        ToolButton {
            text: model.title
            property Unit unit: model.dataRole
            onClicked: {
                root.unitSelected(unit)
            }
        }
    }

    ListView {
        id: phonemeGroupList

        height: 300
        width: 200
        model: phonemeGroupModel
        delegate: groupDelegate
    }
}
