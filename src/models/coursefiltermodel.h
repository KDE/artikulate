/*
    SPDX-FileCopyrightText: 2014-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef COURSEFILTERMODEL_H
#define COURSEFILTERMODEL_H

#include <QSortFilterProxyModel>
#include "../core/resources/editablecourseresource.h"
#include "../models/coursemodel.h"
#include "../core/language.h"

class Course;
class CourseModel;
class Language;

class CourseFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(CourseModel *courseModel READ courseModel WRITE setCourseModel NOTIFY courseModelChanged)
    Q_PROPERTY(int filteredCount READ filteredCount NOTIFY filteredCountChanged)
    Q_PROPERTY(Language *language READ language WRITE setLanguage NOTIFY languageChanged)

public:
    explicit CourseFilterModel(QObject *parent = nullptr);
    CourseModel *courseModel() const;
    void setCourseModel(CourseModel *courseModel);
    int filteredCount() const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    void setLanguage(Language *language);
    Language *language() const;
    Q_INVOKABLE QVariant course(int row) const;

Q_SIGNALS:
    void courseModelChanged();
    void filteredCountChanged();
    void languageChanged();

private:
    CourseModel *m_courseModel {nullptr};
    Language *m_language {nullptr};
};

#endif
