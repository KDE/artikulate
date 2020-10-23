/*
 *  SPDX-FileCopyrightText: 2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "unitfiltermodel.h"
#include "models/unitmodel.h"


#include "artikulate_debug.h"
#include <KLocalizedString>

UnitFilterModel::UnitFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_unitModel(nullptr)
{
}

UnitModel *UnitFilterModel::unitModel() const
{
    return m_unitModel;
}

void UnitFilterModel::setUnitModel(UnitModel *unitModel)
{
    if (unitModel == m_unitModel) {
        return;
    }

    m_unitModel = unitModel;
    setSourceModel(m_unitModel);
    sort(0);
    emit unitModelChanged();
}

int UnitFilterModel::filteredCount() const
{
    return rowCount();
}

bool UnitFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    return QSortFilterProxyModel::lessThan(left, right);
}

bool UnitFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    return sourceModel()->data(index, UnitModel::ContainsTrainingData).toBool();
}
