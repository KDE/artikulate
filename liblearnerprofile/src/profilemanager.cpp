/*
    SPDX-FileCopyrightText: 2013-2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "profilemanager.h"
#include "learner.h"
#include "storage.h"

#include "liblearner_debug.h"
#include <KConfigCore/KConfig>
#include <KConfigCore/KConfigGroup>
#include <KLocalizedString>
#include <QFileDialog>
#include <QList>
#include <memory>

using namespace LearnerProfile;

/// BEGIN: ProfileManagerPrivate
namespace LearnerProfile
{
class ProfileManagerPrivate
{
public:
    ProfileManagerPrivate();
    ~ProfileManagerPrivate()
    {
    }

    void sync();

    QList<Learner *> m_profiles;
    Learner *m_activeProfile;
    QList<LearningGoal *> m_goals;
    std::unique_ptr<KConfig> m_config;
    Storage m_storage;
};
}

LearnerProfile::ProfileManagerPrivate::ProfileManagerPrivate()
    : m_profiles(QList<Learner *>())
    , m_activeProfile(nullptr)
    , m_config(new KConfig(QStringLiteral("learnerprofilerc")))
{
    // load all profiles from storage
    m_goals.append(m_storage.loadGoals());
    m_profiles.append(m_storage.loadProfiles(m_goals));

    // set last used profile
    KConfigGroup activeProfileGroup(m_config.get(), "ActiveProfile");
    int lastProfileId = activeProfileGroup.readEntry("profileId", "0").toInt();
    QList<int> activeGoalsCategories = activeProfileGroup.readEntry("activeGoalsCategories", QList<int>());
    QList<QString> activeGoalsIdentifiers = activeProfileGroup.readEntry("activeGoalsIdentifiers", QList<QString>());
    foreach (Learner *learner, m_profiles) {
        if (learner->identifier() == lastProfileId) {
            m_activeProfile = learner;
            // set active goals
            if (activeGoalsCategories.count() == activeGoalsIdentifiers.count()) {
                for (int i = 0; i < activeGoalsCategories.count(); ++i) {
                    m_activeProfile->setActiveGoal(static_cast<Learner::Category>(activeGoalsCategories.at(i)), activeGoalsIdentifiers.at(i));
                }
            } else {
                qCCritical(LIBLEARNER_LOG()) << "Inconsistent goal category / identifier pairs found: aborting.";
            }
            break;
        }
    }
    if (m_activeProfile == nullptr) {
        qCDebug(LIBLEARNER_LOG()) << "No last active profile found, falling back to first found profile";
        if (m_profiles.size() > 0) {
            m_activeProfile = m_profiles.at(0);
        }
    }
}

void ProfileManagerPrivate::sync()
{
    // sync last used profile data
    if (m_activeProfile) {
        KConfigGroup activeProfileGroup(m_config.get(), "ActiveProfile");
        activeProfileGroup.writeEntry("profileId", m_activeProfile->identifier());

        // compute activer learning goals by category
        QList<int> goalCatogries;
        QList<QString> goalIdentifiers;
        // compute used goals
        foreach (LearningGoal *goal, m_activeProfile->goals()) {
            if (!goalCatogries.contains(static_cast<int>(goal->category()))) {
                goalCatogries.append(static_cast<int>(goal->category()));
            }
        }
        // compute active goals
        foreach (int category, goalCatogries) {
            goalIdentifiers.append(m_activeProfile->activeGoal(static_cast<Learner::Category>(category))->identifier());
        }
        activeProfileGroup.writeEntry("activeGoalsCategories", goalCatogries);
        activeProfileGroup.writeEntry("activeGoalsIdentifiers", goalIdentifiers);
    } else {
        qCCritical(LIBLEARNER_LOG()) << "No active profile selected, aborting sync.";
    }
    m_config->sync();

    // TODO only sync changed learner
    foreach (Learner *learner, m_profiles) {
        m_storage.storeProfile(learner);
    }
}
/// END: ProfileManagerPrivate

ProfileManager::ProfileManager(QObject *parent)
    : QObject(parent)
    , d(new ProfileManagerPrivate)
{
    connect(this, &ProfileManager::profileAdded, this, &ProfileManager::profileCountChanged);
    connect(this, &ProfileManager::profileRemoved, this, &ProfileManager::profileCountChanged);

    foreach (Learner *learner, d->m_profiles) {
        connect(learner, &Learner::goalRemoved, this, &ProfileManager::removeLearningGoal);
    }
}

ProfileManager::~ProfileManager()
{
    for (auto learner : d->m_profiles) {
        learner->deleteLater();
    }
    for (auto goal : d->m_goals) {
        goal->deleteLater();
    }
}

QList<Learner *> ProfileManager::profiles() const
{
    return d->m_profiles;
}

int ProfileManager::profileCount() const
{
    return profiles().length();
}

void ProfileManager::openImageFileDialog()
{
    const QString imagePath = QFileDialog::getOpenFileName(nullptr, i18n("Open Image"), QLatin1String(""), i18n("Image Files (*.png *.jpg *.bmp)"));
    d->m_activeProfile->importImage(imagePath);
}

Learner *ProfileManager::addProfile(const QString &name)
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

    if (activeProfile() == nullptr) {
        setActiveProfile(learner);
    }

    connect(learner, &Learner::goalRemoved, this, &ProfileManager::removeLearningGoal);

    return learner;
}

void ProfileManager::removeProfile(Learner *learner)
{
    int index = d->m_profiles.indexOf(learner);
    if (index < 0) {
        qCWarning(LIBLEARNER_LOG()) << "Profile was not found, aborting";
        return;
    }
    emit profileAboutToBeRemoved(index);
    d->m_profiles.removeAt(index);
    d->m_storage.removeProfile(learner);

    if (d->m_activeProfile == learner) {
        if (d->m_profiles.isEmpty()) {
            setActiveProfile(nullptr);
        } else {
            setActiveProfile(d->m_profiles.at(0));
        }
    }
    emit profileRemoved();
}

void ProfileManager::removeLearningGoal(Learner *learner, LearningGoal *goal)
{
    d->m_storage.removeRelation(learner, goal);
}

Learner *ProfileManager::profile(int index)
{
    if (index < 0 || index >= profiles().count()) {
        return nullptr;
    }
    return profiles().at(index);
}

QList<LearningGoal *> ProfileManager::goals() const
{
    return d->m_goals;
}

LearningGoal *ProfileManager::registerGoal(LearningGoal::Category category, const QString &identifier, const QString &name)
{
    // test whether goal is already registered
    foreach (LearningGoal *cmpGoal, d->m_goals) {
        if (cmpGoal->category() == category && cmpGoal->identifier() == identifier) {
            return cmpGoal;
        }
    }
    LearningGoal *goal = new LearningGoal(category, identifier, this);
    goal->setName(name);
    d->m_goals.append(goal);
    d->m_storage.storeGoal(goal);
    return goal;
}

LearnerProfile::LearningGoal *LearnerProfile::ProfileManager::goal(LearningGoal::Category category, const QString &identifier) const
{
    foreach (LearningGoal *goal, d->m_goals) {
        if (goal->category() == category && goal->identifier() == identifier) {
            return goal;
        }
    }
    return nullptr;
}

void ProfileManager::recordProgress(Learner *learner, LearningGoal *goal, const QString &container, const QString &item, int logPayload, int valuePayload)
{
    if (!learner) {
        qCDebug(LIBLEARNER_LOG()) << "No learner set, no data stored";
        return;
    }
    d->m_storage.storeProgressLog(learner, goal, container, item, logPayload, QDateTime::currentDateTime());
    d->m_storage.storeProgressValue(learner, goal, container, item, valuePayload);
}

QHash<QString, int> ProfileManager::progressValues(Learner *learner, LearningGoal *goal, const QString &container) const
{
    if (!learner || !goal) {
        return QHash<QString, int>();
    }
    return d->m_storage.readProgressValues(learner, goal, container);
}

void ProfileManager::sync()
{
    d->sync();
}

void ProfileManager::sync(Learner *learner)
{
    d->m_storage.storeProfile(learner);
}

Learner *ProfileManager::activeProfile() const
{
    return d->m_activeProfile;
}

void ProfileManager::setActiveProfile(Learner *learner)
{
    if (learner == d->m_activeProfile) {
        return;
    }
    d->m_activeProfile = learner;
    emit activeProfileChanged();
}
