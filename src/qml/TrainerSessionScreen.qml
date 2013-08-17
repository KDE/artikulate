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

    property Unit unit
    property TrainingSession session

    width: 400
    height: 200

    Row {
        height: (session.currentPhrase != null) ? session.currentPhrase.height : 0
        spacing: 20
        TrainerPhrase {
            id: phrase
            width: root.width - trainingControls.width - 50
            phrase: session.currentPhrase
        }

        Column {
            id: trainingControls
            width: Math.max(buttonApply.width, buttonCancel.width)
            PlasmaComponents.ToolButton {
                id: buttonApply
                height: 48
                iconSource: "dialog-ok-apply"
                text: i18n("Correctly Pronounced")
                onClicked: {
                    session.next(TrainingSession.Complete)
                }
            }
            PlasmaComponents.ToolButton {
                id: buttonStepOver
                height: 48
                iconSource: "object-rotate-left"
                text: i18n("Retry Later")
                onClicked: {
                    session.next(TrainingSession.StepOver)
                }
            }
            PlasmaComponents.ToolButton {
                id: buttonCancel
                height: 48
                iconSource: "dialog-cancel"
                text: i18n("Not Correctly Pronounced")
                onClicked: {
                    session.next(TrainingSession.Incomplete)
                }
            }
        }
    }
}
