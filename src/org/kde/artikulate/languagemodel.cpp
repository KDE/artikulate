/*
    SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "languagemodel.h"
#include "languageresourcemodel.h"
#include <KLocalizedString>

LanguageModel::LanguageModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_resourceModel(nullptr)
    , m_view(NonEmptyGhnsOnlyLanguages)
{
    setDynamicSortFilter(true);
}

LanguageResourceModel *LanguageModel::resourceModel() const
{
    return m_resourceModel;
}

void LanguageModel::setView(LanguageResourceView view)
{
    // always change view for resource model
    if (!m_resourceModel) {
        return;
    }
    m_resourceModel->setView(view);

    // set local view variable
    if (m_view == view) {
        return;
    }
    m_view = view;
    emit viewChanged();
}

LanguageModel::LanguageResourceView LanguageModel::view() const
{
    return m_view;
}

void LanguageModel::setResourceModel(LanguageResourceModel *resourceModel)
{
    if (resourceModel != m_resourceModel) {
        m_resourceModel = resourceModel;
        m_resourceModel->setView(m_view);
        setSourceModel(m_resourceModel);
        sort(0);
        emit resourceModelChanged();
    }
}

bool LanguageModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    return QSortFilterProxyModel::lessThan(left, right);
}

QVariant LanguageModel::language(int row) const
{
    return data(index(row, 0), LanguageResourceModel::DataRole);
}
