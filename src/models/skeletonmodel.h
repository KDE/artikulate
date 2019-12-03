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

#ifndef SKELETONMODEL_H
#define SKELETONMODEL_H

#include "artikulatecore_export.h"
#include <QAbstractListModel>
#include <memory>

class IEditableRepository;
class IEditableCourse;
class ICourse;

class ARTIKULATECORE_EXPORT SkeletonModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum skeletonRoles { TitleRole = Qt::UserRole + 1, DescriptionRole, IdRole, DataRole };

    explicit SkeletonModel(QObject *parent = nullptr);
    explicit SkeletonModel(IEditableRepository *repository, QObject *parent = nullptr);
    ~SkeletonModel() override = default;
    QHash<int, QByteArray> roleNames() const override;
    IEditableRepository *resourceRepository() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE QVariant skeleton(int index) const;

protected:
    void setResourceRepository(IEditableRepository *resourceRepository);

private Q_SLOTS:
    void onSkeletonAboutToBeAdded(std::shared_ptr<IEditableCourse> skeleton, int index);
    void onSkeletonAdded();
    void onSkeletonAboutToBeRemoved(int row);
    void onSkeletonRemoved();

private:
    IEditableRepository *m_repository {nullptr};
    QVector<QMetaObject::Connection> m_updateConnections;
};

#endif // SKELETONMODEL_H
