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

#include "languagemodel.h"
#include "models/languageresourcemodel.h"

#include <QSortFilterProxyModel>

#include <KLocale>
#include <KDebug>

LanguageModel::LanguageModel(QObject* parent)
    : QSortFilterProxyModel(parent)
    , m_resourceModel(0)
    , m_view(NonEmptyGhnsOnlyLanguages)
{
    setDynamicSortFilter(true);
}

LanguageResourceModel * LanguageModel::resourceModel() const
{
    return m_resourceModel;
}

void LanguageModel::setView(LanguageResourceView view)
{
    if (m_view == view) {
        return;
    }
    if (!m_resourceModel) {
        return;
    }
    m_resourceModel->setView(view);

    emit viewChanged();
}

LanguageModel::LanguageResourceView LanguageModel::view() const
{
    return m_view;
}

void LanguageModel::setResourceModel(LanguageResourceModel* resourceModel)
{
    if (resourceModel != m_resourceModel) {
        m_resourceModel = resourceModel;
        m_resourceModel->setView(m_view);
        setSourceModel(m_resourceModel);
        sort(0);
        emit resourceModelChanged();
    }
}

bool LanguageModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
    return QSortFilterProxyModel::lessThan(left, right);
}
