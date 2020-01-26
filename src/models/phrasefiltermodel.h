/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@gkde.org>
 *  Copyright 2013       Samikshan Bairagya <samikshan@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef PHRASEFILTERMODEL_H
#define PHRASEFILTERMODEL_H

#include <QSortFilterProxyModel>

class PhraseListModel;

class PhraseFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(PhraseListModel *phraseModel READ phraseModel WRITE setPhraseModel NOTIFY phraseModelChanged)
    Q_PROPERTY(bool hideExcluded READ isHideExcluded WRITE setHideExcluded NOTIFY hideExcludedChanged)
    Q_PROPERTY(bool hideNotRecorded READ isHideNotRecorded WRITE setHideNotRecorded NOTIFY hideNotRecordedChanged)
    Q_PROPERTY(SortOption sortOption READ sortOption WRITE setSortOption NOTIFY sortOptionChanged)
    Q_PROPERTY(int filteredCount READ filteredCount NOTIFY filteredCountChanged)

public:
    Q_ENUMS(SortOption)
    enum SortOption { Id, Type };
    explicit PhraseFilterModel(QObject *parent = nullptr);
    PhraseListModel *phraseModel() const;
    void setPhraseModel(PhraseListModel *phraseModel);
    void setSortOption(SortOption option = Id);
    SortOption sortOption() const;
    int filteredCount() const;
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    void setHideExcluded(bool hide = true);
    bool isHideExcluded() const;
    void setHideNotRecorded(bool hide = true);
    bool isHideNotRecorded() const;

Q_SIGNALS:
    void phraseModelChanged();
    void hideExcludedChanged();
    void hideNotRecordedChanged();
    void sortOptionChanged();
    void filteredCountChanged();

private:
    PhraseListModel *m_phraseModel;
    bool m_hideExcluded;
    bool m_hideNotRecorded;
    SortOption m_sortOption;
};

#endif
