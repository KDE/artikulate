// SPDX-FileCopyrightText: 2018-2024 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

import QtQuick
import QtQuick.Controls as QQC2
import org.kde.kirigami as Kirigami
import org.kde.newstuff as KNS
import org.kde.artikulate

Kirigami.OverlayDrawer {
    id: root

    bottomPadding: 0
    property Kirigami.PageRow pageStack
    property QtObject parentWindow

    // enforce drawer always to be open
    modal: false
    handleVisible: false

    DrawerCourseTreeModel {
        id: drawerCourseTreeModel
        course: TrainingSession.course
        session: TrainingSession
    }

    // ordinary Kirigami actions are filled from training units/phrases
    DrawerTrainingActions {
        id: sessionActions
        model: drawerCourseTreeModel
        onTriggerPhraseView: {
            root.pageStack.clear();
            root.pageStack.push(trainingPageComponent);
        }
        onCurrentIndexChanged: (index) => {
            phraseActionListView.currentIndex = index
        }
    }

    contentItem: Item {
        Column {
            Column {
                id: topContent
                ActionListItem {
                    width: root.contentItem.width
                    action: QQC2.Action {
                        text: i18n("Training")
                        icon.name: "artikulate"
                        onTriggered: {
                            root.pageStack.clear();
                            root.pageStack.push(welcomePageComponent);
                        }
                    }
                }
                Kirigami.Separator {
                    width: root.contentItem.width
                }
            }
            QQC2.ScrollView {
                id: trainingActionScrollView
                clip: true
                width: parent.width
                height: root.height - topContent.height - bottomContent.height
                ListView {
                    id: phraseActionListView
                    model: sessionActions
                    //highlightRangeMode: ListView.ApplyRange
                    highlightFollowsCurrentItem: true
                    delegate: ActionListItem {
                        id: phraseAction
                        required property string actionText
                        required property int kDescendantLevel
                        required property bool kDescendantExpanded
                        required property int index
                        width: trainingActionScrollView.availableWidth
                        text: actionText
                        leftPadding: kDescendantLevel * 20
                        highlighted: phraseAction.index === phraseActionListView.currentIndex
                        action: QQC2.Action {
                            checkable: false
                            onTriggered: {
                                sessionActions.trigger(phraseAction.index)
                                phraseActionListView.currentIndex = phraseAction.index
                            }
                        }
                    }
                }
            }

            Column {
                id: bottomContent
                Kirigami.Separator {
                    width: root.contentItem.width
                }
                ActionListItem {
                    width: root.contentItem.width
                    action: KNS.Action {
                        text: i18n("Download Training")
                        configFile: ":/artikulate/config/artikulate.knsrc"
                        transientParent: root.parentWindow
                        viewMode: KNS.Page.ViewMode.Preview
                        onEntryEvent: function(entry, event) {
                            if (event === KNS.Entry.StatusChangedEvent) {
                                applicationWindow().ghnsCourseDataStatusChanged();
                            }
                        }
                    }
                }
                ActionListItem {
                    width: root.contentItem.width
                    action: Kirigami.Action {
                        text: i18n("About Artikulate")
                        icon.name: "help-about"
                        onTriggered: {
                            if (root.pageStack.layers.depth < 2) {
                                root.pageStack.layers.push(aboutPageComponent)
                            }
                        }
                    }
                }
            }
        }
    }
}
