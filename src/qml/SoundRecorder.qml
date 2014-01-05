/*
 *  Copyright 2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

    property string outputFileUrl : recorderBackend.recordingFile

    signal stopped()

    width: theme.mediumIconSize
    height: theme.mediumIconSize

    Recorder {
        id: recorderBackend
    }

    PlasmaComponents.ToolButton {
        id: recordButton
        height: Math.max(root.width, root.height)
        width: Math.max(root.width, root.height)
        iconSource: "artikulate-media-record"

        onClicked: {
            if (recorderBackend.state == Recorder.RecordingState) {
                console.log("try to stop recording");
                recorderBackend.stop();
                return;
            }
            if (recorderBackend.state == Recorder.StoppedState) {
                console.log("SoundRecorder: start capture");
                recorderBackend.startCapture();
                return;
            }
        }
        Connections {
            target: recorderBackend
            onStateChanged: {
                // update icon
                if (recorderBackend.state == Recorder.RecordingState) {
                    recordButton.iconSource = "artikulate-media-record-active";
                }
                if (recorderBackend.state == Recorder.StoppedState) {
                    recordButton.iconSource = "artikulate-media-record";
                }
            }
        }
    }
}
