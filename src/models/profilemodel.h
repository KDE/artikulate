/*
 *  SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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
