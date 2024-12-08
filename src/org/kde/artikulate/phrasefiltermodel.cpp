/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-FileCopyrightText: 2013 Samikshan Bairagya <samikshan@gmail.com>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "phrasefiltermodel.h"
#include "phraselistmodel.h"
#include <KLocalizedString>

PhraseFilterModel::PhraseFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_phraseModel(nullptr)
    , m_hideExcluded(true)
{
    setHideExcluded(true);
}

PhraseListModel *PhraseFilterModel::phraseModel() const
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

void PhraseFilterModel::setHideNotRecorded(bool hide)
{
    m_hideNotRecorded = hide;
    invalidateFilter();
    emit hideNotRecordedChanged();
}

bool PhraseFilterModel::isHideNotRecorded() const
{
    return m_hideNotRecorded;
}

void PhraseFilterModel::setPhraseModel(PhraseListModel *phraseModel)
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

int PhraseFilterModel::filteredCount() const
{
    return rowCount();
}

bool PhraseFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    if (m_sortOption == Type) {
        return sourceModel()->data(left, PhraseListModel::TypeRole).toInt() < sourceModel()->data(right, PhraseListModel::TypeRole).toInt();
    }
    return QSortFilterProxyModel::lessThan(left, right);
}

bool PhraseFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    int result = true;
    if (m_hideNotRecorded || m_hideExcluded) {
        QModelIndex index = sourceModel()->index(source_row, 0, source_parent);
        bool notRecorded = sourceModel()->data(index, PhraseListModel::SoundFileRole).toUrl().isEmpty();
        bool excluded = sourceModel()->data(index, PhraseListModel::ExcludedRole).toBool();
        result = !(notRecorded || excluded);
    }
    return result;
}
