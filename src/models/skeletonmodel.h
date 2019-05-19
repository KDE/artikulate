/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include <QAbstractListModel>

class ContributorRepository;
class ICourse;
class Skeleton;
class Language;
class QSignalMapper;

class SkeletonModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(ContributorRepository *repository READ resourceRepository WRITE setResourceRepository NOTIFY resourceRepositoryChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum courseRoles {
        TitleRole = Qt::UserRole + 1,
        DescriptionRole,
        IdRole,
        DataRole
    };

    explicit SkeletonModel(QObject *parent = nullptr);
    void setResourceRepository(ContributorRepository *repository);
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int,QByteArray> roleNames() const override;
    ContributorRepository * resourceRepository() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int count() const;
    Q_INVOKABLE QVariant skeleton(int index) const;

Q_SIGNALS:
    void skeletonChanged(int index);
    void resourceRepositoryChanged();
    void countChanged();

private Q_SLOTS:
    void onSkeletonAboutToBeAdded(ICourse *skeleton, int index);
    void onSkeletonAdded();
    void onSkeletonsAboutToBeRemoved(int first, int last);
    void onSkeletonsRemoved();
    void emitSkeletonChanged(int row);

private:
    void updateMappings();
    ContributorRepository *m_repository;
    QSignalMapper *m_signalMapper;
};

#endif // SKELETONMODEL_H
