
// SPDX-FileCopyrightText: 2018-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL

#include "drawertrainingactions.h"
#include "drawercoursetreemodel.h"
#include <KLocalizedString>
#include <QAction>
#include <QList>

DrawerTrainingActions::DrawerTrainingActions(QObject *parent)
    : KDescendantsProxyModel{parent}
{
    setExpandsByDefault(false);
}

void DrawerTrainingActions::setSourceModel(QAbstractItemModel *model)
{
    KDescendantsProxyModel::setSourceModel(model);
    DrawerCourseTreeModel *treeModel = qobject_cast<DrawerCourseTreeModel *>(sourceModel());
    if (treeModel) {
        connect(treeModel, &DrawerCourseTreeModel::currentIndexChanged, this, [this](const QModelIndex &currentIndex) {
            if (!currentIndex.parent().isValid()) {
                qCritical() << "handling model index that does not belong to phrase, aborting";
                return;
            }
            expandSourceIndex(currentIndex.parent());
            Q_EMIT currentIndexChanged(mapFromSource(currentIndex).row());
        });
    }
}

void DrawerTrainingActions::trigger(int index)
{
    const auto sourceIndex = mapToSource(createIndex(index, 0));
    if (isSourceIndexExpanded(sourceIndex)) {
        collapseSourceIndex(sourceIndex);
    } else {
        expandSourceIndex(sourceIndex);
    }
    DrawerCourseTreeModel *treeModel = qobject_cast<DrawerCourseTreeModel *>(sourceModel());
    if (treeModel) {
        treeModel->trigger(sourceIndex);
    }

    Q_EMIT triggerPhraseView();
}

QHash<int, QByteArray> DrawerTrainingActions::roleNames() const
{
    auto roles = KDescendantsProxyModel::roleNames();
    roles[Qt::ItemDataRole::DisplayRole] = "actionText";
    roles[Qt::ItemDataRole::ToolTipRole] = "tooltip";
    roles[Qt::ItemDataRole::CheckStateRole] = "selected";
    roles[DrawerTrainingActions::Action] = "drawerAction";
    return roles;
}
