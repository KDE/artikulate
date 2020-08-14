/*
 *  SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef LANGUAGEMODEL_H
#define LANGUAGEMODEL_H

#include <QSortFilterProxyModel>

class LanguageResourceModel;
class Language;

class LanguageModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(LanguageResourceModel *resourceModel READ resourceModel WRITE setResourceModel NOTIFY resourceModelChanged)
    Q_PROPERTY(LanguageResourceView view READ view WRITE setView NOTIFY viewChanged)

public:
    enum LanguageResourceView { NonEmptyContributorOnlyResources, NonEmptyGhnsOnlyLanguages, NonEmptyLanguages, AllLanguages };
    Q_ENUM(LanguageResourceView)

    explicit LanguageModel(QObject *parent = nullptr);
    LanguageResourceModel *resourceModel() const;
    void setResourceModel(LanguageResourceModel *resourceModel);
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    void setView(LanguageResourceView view);
    LanguageResourceView view() const;
    Q_INVOKABLE QVariant language(int index) const;

Q_SIGNALS:
    void resourceModelChanged();
    void viewChanged();

private:
    LanguageResourceModel *m_resourceModel;
    LanguageResourceView m_view;
};

#endif
