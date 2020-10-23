/*
 *  SPDX-FileCopyrightText: 2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "coursefiltermodel.h"
#include "../core/language.h"
#include "artikulate_debug.h"
#include "models/coursemodel.h"
#include <KLocalizedString>
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
