/*
    SPDX-FileCopyrightText: 2013-2024 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef PROFILEMODEL_H
#define PROFILEMODEL_H

#include "../../liblearnerprofile/src/profilemanager.h"
#include <QAbstractListModel>
#include <QQmlEngine>

namespace LearnerProfile
{
class Learner;
class ProfileManager;
}

#ifndef OPAQUE_PTR_Learner
#define OPAQUE_PTR_Learner
Q_DECLARE_OPAQUE_POINTER(LearnerProfile::Learner *)
#endif
class QSignalMapper;

class ProfileModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(LearnerProfile::ProfileManager *profileManager READ profileManager WRITE setProfileManager)

    QML_ELEMENT

public:
    enum profileRoles {
        NameRole = Qt::UserRole + 1,
        IdRole,
        DataRole
    };

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
    void onProfileChanged(int row);

private:
    void updateMappings();
    LearnerProfile::ProfileManager *m_profileManager;
    QSignalMapper *m_signalMapper;
};

#endif // PROFILEMODEL_H
