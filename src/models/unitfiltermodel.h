/*
    SPDX-FileCopyrightText: 2014-2015 Andreas Cord-Landwehr <cordlandwehr@gkde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef UNITFILTERMODEL_H
#define UNITFILTERMODEL_H

#include <QSortFilterProxyModel>

class UnitModel;

class UnitFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(UnitModel *unitModel READ unitModel WRITE setUnitModel NOTIFY unitModelChanged)
    Q_PROPERTY(int filteredCount READ filteredCount NOTIFY filteredCountChanged)

public:
    explicit UnitFilterModel(QObject *parent = nullptr);
    UnitModel *unitModel() const;
    void setUnitModel(UnitModel *unitModel);
    int filteredCount() const;
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

Q_SIGNALS:
    void unitModelChanged();
    void viewChanged();
    void sortOptionChanged();
    void filteredCountChanged();

private:
    UnitModel *m_unitModel;
};

#endif
