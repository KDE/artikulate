/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@gkde.org>
 *  Copyright 2013       Samikshan Bairagya <samikshan@gmail.com>
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

#ifndef PHRASEFILTERMODEL_H
#define PHRASEFILTERMODEL_H

#include <QSortFilterProxyModel>

class PhraseListModel;
class QSignalMapper;

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
    enum SortOption {
        Id,
        Type
    };
    explicit PhraseFilterModel(QObject *parent = nullptr);
    PhraseListModel * phraseModel() const;
    void setPhraseModel(PhraseListModel* phraseModel);
    void setSortOption(SortOption option = Id);
    SortOption sortOption() const;
    int filteredCount() const;
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const Q_DECL_OVERRIDE;
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_OVERRIDE;
    void setHideExcluded(bool hide=true);
    bool isHideExcluded() const;
    void setHideNotRecorded(bool hide=true);
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
