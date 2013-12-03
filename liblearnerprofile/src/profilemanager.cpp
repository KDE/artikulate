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

#include "profilemanager_p.h"
#include "profilemanager.h"
#include "learner.h"

#include <QObject>
#include <QList>
#include <KDebug>

using namespace LearnerProfile;

ProfileManager::ProfileManager(QObject* parent)
    : QObject(parent)
    , d(new ProfileManagerPrivate)
{
    connect (this, SIGNAL(profileAdded(Learner*,int)), this, SIGNAL(profileCountChanged()));
    connect (this, SIGNAL(profileRemoved()), this, SIGNAL(profileCountChanged()));

    foreach (Learner *learner, d->m_profiles) {
        connect (learner, SIGNAL(goalRemoved(Learner*,LearningGoal*)),
                this, SLOT(removeLearningGoal(Learner*,LearningGoal*)));
    }
}

ProfileManager::~ProfileManager()
{
    foreach (Learner *learner, d->m_profiles) {
        learner->deleteLater();
    }
}

QList< Learner* > ProfileManager::profiles() const
{
    return d->m_profiles;
}

int ProfileManager::profileCount() const
{
    return profiles().length();
}

Learner * ProfileManager::addProfile(const QString &name)
{
    Learner *learner = new Learner(this);
    learner->setName(name);

    // set id
    int maxUsedId = 0;
    foreach (Learner *cpLearner, d->m_profiles) {
        if (cpLearner->identifier() >= maxUsedId) {
            maxUsedId = cpLearner->identifier();
        }
    }
    learner->setIdentifier(maxUsedId + 1);

    d->m_profiles.append(learner);
    d->m_storage.storeProfile(learner);
    emit profileAdded(learner, d->m_profiles.count() - 1);

    if (activeProfile() == 0) {
        setActiveProfile(learner);
    }

    connect (learner, SIGNAL(goalRemoved(Learner*,LearningGoal*)),
            this, SLOT(removeLearningGoal(Learner*,LearningGoal*)));

    return learner;
}

void ProfileManager::removeProfile(Learner *learner)
{
    int index = d->m_profiles.indexOf(learner);
    if (index < 0) {
        kWarning() << "Profile was not found, aborting";
        return;
    }
    emit profileAboutToBeRemoved(index);
    d->m_profiles.removeAt(index);
    d->m_storage.removeProfile(learner);

    if (d->m_activeProfile == learner) {
        if (d->m_profiles.count() == 0) {
            setActiveProfile(0);
        }
        else {
            setActiveProfile(d->m_profiles.at(0));
        }
    }
    emit profileRemoved();
}

void ProfileManager::removeLearningGoal(Learner* learner, LearningGoal* goal)
{
    d->m_storage.removeRelation(learner, goal);
}

Learner * ProfileManager::profile(int index)
{
    if (index < 0 || index >= profiles().count()) {
        return 0;
    }
    return profiles().at(index);
}

QList< LearningGoal* > ProfileManager::goals() const
{
    return d->m_goals;
}

void ProfileManager::registerGoal(LearningGoal::Category category, const QString &identifier, const QString &name)
{
    // test whether goal is already registered
    foreach (LearningGoal *cmpGoal, d->m_goals) {
        if (cmpGoal->category() == category && cmpGoal->identifier() == identifier) {
            return;
        }
    }
    LearningGoal *goal = new LearningGoal(category, identifier, this);
    goal->setName(name);
    d->m_goals.append(goal);
    d->m_storage.storeGoal(goal);
}

void ProfileManager::sync()
{
    d->sync();
}

void ProfileManager::sync(Learner *learner)
{
    d->m_storage.storeProfile(learner);
}

Learner * ProfileManager::activeProfile() const
{
    return d->m_activeProfile;
}

void ProfileManager::setActiveProfile(Learner* learner)
{
    if (learner == d->m_activeProfile) {
        return;
    }
    d->m_activeProfile = learner;
    emit activeProfileChanged();
}
