/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) version 3, or any
 *  later version accepted by the membership of KDE e.V. (or its
 *  successor approved by the membership of KDE e.V.), which shall
 *  act as a proxy defined in Section 6 of version 3 of the license.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef PROFILEMANAGER_H
#define PROFILEMANAGER_H

#include "liblearnerprofile_export.h"
#include <QObject>

namespace LearnerProfile
{
class ProfileManagerPrivate;
class Learner;

/**
 * \class ProfileManager
 */
class LIBLEARNERPROFILE_EXPORT ProfileManager : public QObject
{
    Q_OBJECT

public:
    explicit ProfileManager(QObject *parent = 0);
    ~ProfileManager();

    QList< Learner* > profiles() const;
    Learner * addProfile(const QString &name);
    void sync();
    void setActiveProfile(Learner *learner);
    Learner * activeProfile() const;

Q_SIGNALS:
    void activeProfileChanged();

private:
    Q_DISABLE_COPY(ProfileManager)
    const QScopedPointer<ProfileManagerPrivate> d;
};
}

#endif // PROFILEMANAGER_H
