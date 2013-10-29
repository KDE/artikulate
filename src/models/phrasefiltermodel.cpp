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

#include "phrasefiltermodel.h"
#include "models/phrasemodel.h"

#include <QSortFilterProxyModel>

#include <KLocale>
#include <KDebug>

PhraseFilterModel::PhraseFilterModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    , m_phraseModel(0)
    , m_hideExcluded(true)
{
    setHideExcluded(true);
}

PhraseModel * PhraseFilterModel::phraseModel() const
{
    return m_phraseModel;
}

void PhraseFilterModel::setHideExcluded(bool hide)
{
    m_hideExcluded = hide;
    invalidateFilter();
    emit hideExcludedChanged();
}

bool PhraseFilterModel::isHideExcluded() const
{
    return m_hideExcluded;
}

void PhraseFilterModel::setPhraseModel(PhraseModel* phraseModel)
{
    if (phraseModel == m_phraseModel) {
        return;
    }

    m_phraseModel = phraseModel;
    setSourceModel(m_phraseModel);
    sort(0);

    emit phraseModelChanged();

}

void PhraseFilterModel::setSortOption(PhraseFilterModel::SortOption option)
{
    m_sortOption = option;
    invalidateFilter();
    emit sortOptionChanged();
}

PhraseFilterModel::SortOption PhraseFilterModel::sortOption() const
{
    return m_sortOption;
}

bool PhraseFilterModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    if (m_sortOption == Type) {
        return sourceModel()->data(left, PhraseModel::TypeRole).toInt() < sourceModel()->data(right, PhraseModel::TypeRole).toInt();
    }
    return QSortFilterProxyModel::lessThan(left, right);
}

bool PhraseFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (m_hideExcluded == true) {
        // return true if not excluded
        QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
        !sourceModel()->data(index, PhraseModel::ExcludedRole).toBool();
        return !sourceModel()->data(index, PhraseModel::ExcludedRole).toBool();
    }
    return true;
}
