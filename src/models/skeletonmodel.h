/*
 *  SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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
