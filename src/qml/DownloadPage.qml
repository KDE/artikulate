/*
 *  Copyright 2018  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

import QtQuick 2.1
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.1 as QQC2
import org.kde.kirigami 2.7 as Kirigami
import org.kde.newstuff 1.0 as KNS

Kirigami.Page {
    id: root

    /**
     * emitted whenever GHNS resources changed
     */
    signal statusChanged();

    title: i18n("Download Training Material")
    background: Rectangle {
        color: Kirigami.Theme.backgroundColor
        Kirigami.Theme.colorSet: Kirigami.Theme.View
//        Kirigami.Theme.inherit: false
    }

    Component {
        id: courseDownloadItem

        Kirigami.AbstractListItem {
            id: listItem
            height: 50
            width: parent.width
            text: model.name
            readonly property var status: model.status
            onStatusChanged: {
                root.statusChanged();
            }
            checkable: false
            RowLayout {
                id: layout
                spacing: Kirigami.Units.smallSpacing*2

                Kirigami.Icon {
                    height: Kirigami.Units.iconSizes.smallMedium;
                    width: height;
                    SequentialAnimation on opacity {
                        loops: Animation.Infinite;
                        running: model.status == KNS.ItemsModel.InstallingStatus || model.status == KNS.ItemsModel.UpdatingStatus
                        NumberAnimation { to: 0; duration: 500; }
                        NumberAnimation { to: 1; duration: 500; }
                        onRunningChanged: { if (!running) parent.opacity = 1; }
                    }
                    source: {
                        // use complete list of KNS status messages
                        if (model.status == KNS.ItemsModel.InvalidStatus) return "emblem-error";
                        if (model.status == KNS.ItemsModel.DownloadableStatus) return "vcs-added";
                        if (model.status == KNS.ItemsModel.InstalledStatus) return "vcs-normal";
                        if (model.status == KNS.ItemsModel.UpdateableStatus) return "vcs-update-required";
                        if (model.status == KNS.ItemsModel.DeletedStatus) return "vcs-added";
                        if (model.status == KNS.ItemsModel.InstallingStatus) return "vcs-locally-modified";
                        if (model.status == KNS.ItemsModel.UpdatingStatus) return "vcs-locally-modified";
                        return "emblem-error";
                    }
                }

                QQC2.Label {
                    id: labelItem
                    Layout.fillWidth: true
                    text: listItem.text
                    color: layout.indicateActiveFocus && (listItem.highlighted || listItem.checked || listItem.pressed) ? listItem.activeTextColor : listItem.textColor
                    elide: Text.ElideRight
                    font: listItem.font
                }

                QQC2.Button {
                    visible: (model.status == KNS.ItemsModel.UpdateableStatus) ? true : false;
                    text: i18nc("@action:button", "Update")
                    onClicked: newStuffModel.installItem(model.index, model.PayloadRole)
                }
                QQC2.Button {
                    visible: (model.status == KNS.ItemsModel.DownloadableStatus || model.status == KNS.ItemsModel.DeletedStatus) ? true : false;
                    text: i18nc("@action:button", "Install")
                    onClicked: newStuffModel.installItem(model.index, model.PayloadRole)
                }
                QQC2.Button {
                    visible: (model.status == KNS.ItemsModel.InstalledStatus || model.status == KNS.ItemsModel.UpdateableStatus) ? true : false;
                    text: i18nc("@action:button", "Remove")
                    onClicked: newStuffModel.uninstallItem(model.index)
                }
            }
        }
    }

    ColumnLayout {
        ListView {
            id: listView
            width: root.width - 40
            height: 50 * listView.count
            delegate: courseDownloadItem
            model: KNS.ItemsModel {
                id: newStuffModel;
                engine: newStuffEngine;

            }
            KNS.Engine {
                id: newStuffEngine;
                configFile: ":/artikulate/config/artikulate.knsrc";
                onMessage: console.log("KNS Message: " + message);
                onIdleMessage: console.log("KNS Idle: " + message);
                onBusyMessage: console.log("KNS Busy: " + message);
                onErrorMessage: console.log("KNS Error: " + message);
           }
       }
   }
}
