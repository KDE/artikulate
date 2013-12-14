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
#include <QMultiMap>

namespace LearnerProfile
{
class Learner;
class LearningGoal;

class ProfileManagerPrivate {

public:
    ProfileManagerPrivate();
    ~ProfileManagerPrivate() {}

    void sync();

    QList<Learner*> m_profiles;
    Learner *m_activeProfile;
    QList<LearningGoal*> m_goals;
    KConfig *m_config;
    Storage m_storage;
};

ProfileManagerPrivate::ProfileManagerPrivate()
    : m_profiles(QList<Learner*>())
    , m_activeProfile(0)
    , m_config(0)
{
    // load all profiles from storage
    m_goals.append(m_storage.loadGoals());
    m_profiles.append(m_storage.loadProfiles(m_goals));

    // set last used profile
    m_config = new KConfig("learnerprofilerc");
    KConfigGroup activeProfileGroup(m_config, "ActiveProfile");
    int lastProfileId = activeProfileGroup.readEntry("profileId", "0").toInt();
    QList<int> activeGoalsCategories = activeProfileGroup.readEntry("activeGoalsCategories", QList<int>());
    QList<QString> activeGoalsIdentifiers = activeProfileGroup.readEntry("activeGoalsIdentifiers", QList<QString>());
    foreach (Learner *learner, m_profiles) {
        if (learner->identifier() == lastProfileId) {
            m_activeProfile = learner;
            // set active goals
            if (activeGoalsCategories.count() == activeGoalsIdentifiers.count()) {
                for (int i = 0; i < activeGoalsCategories.count(); ++i) {
                    m_activeProfile->setActiveGoal(
                        (Learner::Category) activeGoalsCategories.at(i),
                        activeGoalsIdentifiers.at(i));
                }
            } else {
                kError() << "Inconsistent goal category / identifier pairs found: aborting.";
            }
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

        // compute activer learning goals by category
        QList<int> goalCatogries;
        QList<QString> goalIdentifiers;
        // compute used goals
        foreach (LearningGoal *goal, m_activeProfile->goals()) {
            if (!goalCatogries.contains((int) goal->category())) {
                goalCatogries.append((int) goal->category());
            }
        }
        // compute active goals
        foreach (int category, goalCatogries) {
            goalIdentifiers.append(m_activeProfile->activeGoal((Learner::Category) category)->identifier());
        }
        activeProfileGroup.writeEntry("activeGoalsCategories", goalCatogries);
        activeProfileGroup.writeEntry("activeGoalsIdentifiers", goalIdentifiers);
    }
    else {
        kError() << "No active profile selected, aborting sync.";
    }
    m_config->sync();

    //TODO only sync changed learner
    foreach (Learner *learner, m_profiles) {
        m_storage.storeProfile(learner);
    }
}

}
