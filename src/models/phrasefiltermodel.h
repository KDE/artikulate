/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gkde.org>
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

class PhraseModel;
class QSignalMapper;

class PhraseFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(PhraseModel *phraseModel READ phraseModel WRITE setPhraseModel NOTIFY phraseModelChanged)
    Q_PROPERTY(bool hideExcluded READ isHideExcluded WRITE setHideExcluded NOTIFY hideExcludedChanged)
    Q_PROPERTY(SortOption sortOption READ sortOption WRITE setSortOption NOTIFY sortOptionChanged)

public:
    Q_ENUMS(SortOption)
    enum SortOption {
        Id,
        Type
    };
    explicit PhraseFilterModel(QObject *parent = 0);
    PhraseModel * phraseModel() const;
    void setPhraseModel(PhraseModel* phraseModel);
    void setSortOption(SortOption option = Id);
    SortOption sortOption() const;
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    void setHideExcluded(bool hide=true);
    bool isHideExcluded() const;

signals:
    void phraseModelChanged();
    void hideExcludedChanged();
    void sortOptionChanged();

private:
    PhraseModel *m_phraseModel;
    bool m_hideExcluded;
    SortOption m_sortOption;
};

#endif
