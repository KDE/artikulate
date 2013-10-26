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
#include <QUuid>
#include <KDebug>

using namespace LearnerProfile;

ProfileManager::ProfileManager(QObject* parent)
    : QObject(parent)
    , d(new ProfileManagerPrivate)
{

}

ProfileManager::~ProfileManager()
{

}

QList< Learner* > ProfileManager::profiles() const
{
    return d->m_profiles;
}

Learner * ProfileManager::addProfile(const QString &name)
{
    Learner *learner = new Learner(this);
    learner->setName(name);

    //compare that profile id is unique
    bool unique;
    do {
        learner->setIdentifier(QUuid::createUuid());
        unique = true;
        foreach (Learner *cpLearner, profiles()) {
            if (learner->identifier() == cpLearner->identifier()) {
                unique = false;
                kWarning() << "Learner id already use, computing new one.";
            }
        }
    } while (!unique);

    d->m_profiles.append(learner);


    return learner;
}

void ProfileManager::sync()
{
    d->sync();
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
