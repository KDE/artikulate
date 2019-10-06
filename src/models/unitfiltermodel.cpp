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

#include "unitfiltermodel.h"
#include "models/unitmodel.h"

#include <QSortFilterProxyModel>

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
