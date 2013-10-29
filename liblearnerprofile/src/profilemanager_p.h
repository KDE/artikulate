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
#include "storage.h"

#include <KDebug>
#include <KConfig>
#include <KConfigGroup>
#include <KStandardDirs>

#include <QString>
#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

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
    Storage m_storage;
};

ProfileManagerPrivate::ProfileManagerPrivate()
    : m_profiles(QList<Learner*>())
    , m_activeProfile(0)
    , m_config(0)
{
    // load all profiles from storage
    m_profiles.append(m_storage.loadProfiles());

    // set last used profile
    m_config = new KConfig("learnerprofilerc");
    KConfigGroup activeProfileGroup(m_config, "ActiveProfile");
    int lastProfileId = activeProfileGroup.readEntry("profileId", "0").toInt();
    foreach (Learner *learner, m_profiles) {
        if (learner->identifier() == lastProfileId) {
            m_activeProfile = learner;
            break;
        }
    }
    if (m_activeProfile == 0) {
        kDebug() << "No last active profile found, falling back to first found profile";
        if (m_profiles.size() > 0) {
            m_activeProfile = m_profiles.at(0);
        }
    }
}

void ProfileManagerPrivate::sync()
{
    // sync last used profile data
    if (m_activeProfile) {
        KConfigGroup activeProfileGroup(m_config, "ActiveProfile");
        activeProfileGroup.writeEntry("profileId", m_activeProfile->identifier());
    }
    else {
        kError() << "No active profile selected, aborting sync.";
    }
    m_config->sync();

    // note: not storage operations necessary, always in sync
}

}
