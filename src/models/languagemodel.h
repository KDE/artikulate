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

#ifndef LANGUAGEMODEL_H
#define LANGUAGEMODEL_H

#include <QSortFilterProxyModel>

class LanguageResourceModel;
class Language;
class QSignalMapper;

class LanguageModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(LanguageResourceModel *resourceModel READ resourceModel WRITE setResourceModel NOTIFY resourceModelChanged)
    Q_PROPERTY(LanguageResourceView view READ view WRITE setView NOTIFY viewChanged)

public:
    Q_ENUMS(LanguageResourceView);
    enum LanguageResourceView {
        NonEmptyContributorOnlyResources,
        NonEmptyGhnsOnlyLanguages,
        NonEmptyLanguages,
        AllLanguages
    };

    explicit LanguageModel(QObject *parent = 0);
    LanguageResourceModel * resourceModel() const;
    void setResourceModel(LanguageResourceModel *resourceModel);
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    void setView(LanguageResourceView view);
    LanguageResourceView view() const;

signals:
    void resourceModelChanged();
    void viewChanged();

private:
    LanguageResourceModel *m_resourceModel;
    LanguageResourceView m_view;
};

#endif
