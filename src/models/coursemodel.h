/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef COURSEMODEL_H
#define COURSEMODEL_H

#include "artikulatecore_export.h"
#include <QAbstractListModel>
#include <memory>

class IResourceRepository;
class ICourse;
class Language;

class ARTIKULATECORE_EXPORT CourseModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum courseRoles { TitleRole = Qt::UserRole + 1, I18nTitleRole, DescriptionRole, IdRole, LanguageRole, DataRole };

    explicit CourseModel(QObject *parent = nullptr);
    explicit CourseModel(IResourceRepository *repository, QObject *parent = nullptr);
    ~CourseModel() override = default;
    QHash<int, QByteArray> roleNames() const override;
    IResourceRepository *resourceRepository() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE QVariant course(int index) const;

protected:
    void setResourceRepository(IResourceRepository *resourceRepository);

private Q_SLOTS:
    void onCourseAboutToBeAdded(std::shared_ptr<ICourse> course, int index);
    void onCourseAdded();
    void onCourseAboutToBeRemoved(int row);

private:
    IResourceRepository *m_resourceRepository {nullptr};
    QVector<QMetaObject::Connection> m_updateConnections;
};

#endif // COURSEMODEL_H
