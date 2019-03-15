/*
 *  Copyright 2014-2015  Andreas Cord-Landwehr <cordlandwehr@gkde.org>
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
class QSignalMapper;

class CourseFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(CourseModel *courseModel READ courseModel WRITE setCourseModel NOTIFY courseModelChanged)
    Q_PROPERTY(CourseResourceView view READ view WRITE setView NOTIFY viewChanged)
    Q_PROPERTY(int filteredCount READ filteredCount NOTIFY filteredCountChanged)

public:
    Q_ENUMS(CourseResourceView);
    enum CourseResourceView {
        OnlyGetHotNewStuffResources = 1,
        OnlyContributorResources = 2,
        AllResources = 3
    };
    explicit CourseFilterModel(QObject *parent = nullptr);
    CourseModel * courseModel() const;
    void setCourseModel(CourseModel* courseModel);
    int filteredCount() const;
    virtual bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    void setView(CourseResourceView view);
    CourseResourceView view() const;
    Q_INVOKABLE QVariant course(int row) const;

Q_SIGNALS:
    void courseModelChanged();
    void viewChanged();
    void sortOptionChanged();
    void filteredCountChanged();

private:
    CourseModel *m_courseModel;
    CourseResourceView m_view;
};

#endif
