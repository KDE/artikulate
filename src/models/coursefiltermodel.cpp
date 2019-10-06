/*
 *  Copyright 2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "coursefiltermodel.h"
#include "../core/language.h"
#include "artikulate_debug.h"
#include "models/coursemodel.h"
#include <KLocalizedString>
#include <QSortFilterProxyModel>
#include <QVariant>

CourseFilterModel::CourseFilterModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_courseModel(nullptr)
{
}

CourseModel *CourseFilterModel::courseModel() const
{
    return m_courseModel;
}

Language *CourseFilterModel::language() const
{
    return m_language;
}

void CourseFilterModel::setLanguage(Language *language)
{
    if (m_language == language) {
        return;
    }
    m_language = language;
    emit languageChanged();
    invalidateFilter();
    emit filteredCountChanged();
}

void CourseFilterModel::setCourseModel(CourseModel *courseModel)
{
    if (courseModel == m_courseModel) {
        return;
    }
    m_courseModel = courseModel;

    setSourceModel(m_courseModel);
    sort(0);
    emit courseModelChanged();
    emit filteredCountChanged();
}

int CourseFilterModel::filteredCount() const
{
    return rowCount();
}

bool CourseFilterModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    return QSortFilterProxyModel::lessThan(left, right);
}

bool CourseFilterModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    if (m_language == nullptr) {
        return true;
    } else if (sourceModel()->data(index, CourseModel::LanguageRole).value<Language *>()->id() == m_language->id()) {
        return true;
    }
    return false;
}

QVariant CourseFilterModel::course(int row) const
{
    return m_courseModel->data(m_courseModel->index(row, 0), CourseModel::DataRole);
}
