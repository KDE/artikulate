/*
 *  Copyright 2014-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef COURSEFILTERMODEL_H
#define COURSEFILTERMODEL_H

#include <QSortFilterProxyModel>

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
    CourseModel * courseModel() const;
    void setCourseModel(CourseModel* courseModel);
    int filteredCount() const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    void setLanguage(Language *language);
    Language * language() const;
    Q_INVOKABLE QVariant course(int row) const;

Q_SIGNALS:
    void courseModelChanged();
    void filteredCountChanged();
    void languageChanged();

private:
    CourseModel *m_courseModel{ nullptr };
    Language *m_language{ nullptr };
};

#endif
