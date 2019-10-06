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

#ifndef PROFILEMODEL_H
#define PROFILEMODEL_H

#include <QAbstractListModel>

namespace LearnerProfile
{
class Learner;
class ProfileManager;
}

class QSignalMapper;

class ProfileModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(LearnerProfile::ProfileManager *profileManager READ profileManager WRITE setProfileManager)

public:
    enum profileRoles { NameRole = Qt::UserRole + 1, IdRole, DataRole };

    explicit ProfileModel(QObject *parent = nullptr);
    /**
     * Reimplemented from QAbstractListModel::roleNames()
     */
    virtual QHash<int, QByteArray> roleNames() const override;
    void setProfileManager(LearnerProfile::ProfileManager *profileManager);
    LearnerProfile::ProfileManager *profileManager() const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

Q_SIGNALS:
    void profileChanged(int index);

private Q_SLOTS:
    void onProfileAdded(LearnerProfile::Learner *learner, int index);
    void onProfileAboutToBeRemoved(int index);
    void emitProfileChanged(int row);

private:
    void updateMappings();
    LearnerProfile::ProfileManager *m_profileManager;
    QSignalMapper *m_signalMapper;
};

#endif // PROFILEMODEL_H
