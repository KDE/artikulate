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

#include "learner.h"

#include <QString>
#include <QList>
#include <KDebug>
#include <KConfig>
#include <KConfigGroup>
#include <KStandardDirs>

namespace LearnerProfile
{
class Learner;

class ProfileManagerPrivate {

public:
    ProfileManagerPrivate();
    ~ProfileManagerPrivate() {}

    void sync();

    QList<Learner*> m_profiles;
    Learner *m_activeProfile;
    KConfig *m_config;
};

ProfileManagerPrivate::ProfileManagerPrivate()
{
    m_config = new KConfig("learnerprofilerc");

    //FIXME reading of profiles not implemented yet
}

void ProfileManagerPrivate::sync()
{
    kDebug() << "Writing profile list to file.";

    //FIXME syncing of profiles not implemented yet

    KConfigGroup activeProfileGroup(m_config, "ActiveProfile");
    activeProfileGroup.writeEntry("profileId", m_activeProfile->identifier());

    m_config->sync();
}

}
