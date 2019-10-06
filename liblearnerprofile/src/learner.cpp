/*
 *  Copyright 2013-2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
#include "learner_p.h"
#include "learninggoal.h"
#include "liblearner_debug.h"
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QPixmap>

using namespace LearnerProfile;

Learner::Learner(QObject *parent)
    : QObject(parent)
    , d(new LearnerPrivate)
{
    connect(this, &Learner::goalAdded, this, &Learner::goalCountChanged);
    connect(this, &Learner::goalRemoved, this, &Learner::goalCountChanged);
}

Learner::~Learner()
{
}

QString Learner::name() const
{
    return d->m_name;
}

void Learner::setName(const QString &name)
{
    if (name == d->m_name) {
        return;
    }
    d->m_name = name;
    emit nameChanged();
}

int Learner::identifier() const
{
    return d->m_identifier;
}

void Learner::setIdentifier(int identifier)
{
    if (identifier == d->m_identifier) {
        return;
    }
    d->m_identifier = identifier;
    emit identifierChanged();
}

QString Learner::imageUrl() const
{
    QString path = d->imagePath();
    if (!QFileInfo::exists(path)) {
        return QString();
    }
    return "file://" + path;
}

void Learner::clearImage()
{
    const QString path {d->imagePath()};
    if (!QFileInfo::exists(path)) {
        return;
    }
    QFile file;
    if (!file.remove(path)) {
        qCCritical(LIBLEARNER_LOG) << "could not remove image:" << path;
    }
    emit imageChanged();
}

void Learner::importImage(const QString &path)
{
    if (!QFileInfo::exists(path)) {
        qCWarning(LIBLEARNER_LOG) << "image path points to a non-existing file, aborting: " << path;
        return;
    }

    // create image directory if it does not exist
    QDir dir;
    if (!dir.exists(d->imageDirectory())) {
        dir.mkdir(d->imageDirectory());
    }

    QPixmap image = QPixmap(path);
    image = image.scaled(120, 120);
    if (!image.save(d->imagePath(), "PNG")) {
        qCCritical(LIBLEARNER_LOG()) << "could not save scaled image to" << d->imagePath();
    }
    emit imageChanged();
    qCDebug(LIBLEARNER_LOG) << "saved scaled image from " << path << " at " << d->imagePath();
}

QList<LearningGoal *> Learner::goals() const
{
    return d->m_goals;
}

void Learner::addGoal(LearnerProfile::LearningGoal *goal)
{
    if (d->m_goals.contains(goal)) {
        return;
    }
    emit goalAboutToBeAdded(goal, d->m_goals.count());
    d->m_goals.append(goal);
    emit goalAdded();
}

void Learner::removeGoal(LearnerProfile::LearningGoal *goal)
{
    int index = d->m_goals.indexOf(goal);
    if (index < 0) {
        qCritical() << "Cannot remove goal, not found: aborting";
        return;
    }
    emit goalAboutToBeRemoved(index);
    d->m_goals.removeAt(index);
    emit goalRemoved(this, goal);
}

bool Learner::hasGoal(LearningGoal *goal) const
{
    foreach (LearningGoal *cmpGoal, d->m_goals) {
        if (goal->identifier() == cmpGoal->identifier()) {
            return true;
        }
    }
    return false;
}

void Learner::setActiveGoal(LearningGoal *goal)
{
    if (d->m_activeGoal.contains(goal->category()) && d->m_activeGoal[goal->category()] == goal) {
        return;
    }

    d->m_activeGoal.insert(goal->category(), goal);
    emit activeGoalChanged();
}

void Learner::setActiveGoal(Learner::Category categoryLearner, const QString &identifier)
{
    // TODO:Qt5 change method parameter to LearningGoal::Category
    // workaround for Q_INVOKABLE access of enum
    LearningGoal::Category category = static_cast<LearningGoal::Category>(categoryLearner);

    if (d->m_activeGoal.contains(category) && d->m_activeGoal[category]->identifier() == identifier) {
        return;
    }

    foreach (LearningGoal *goal, d->m_goals) {
        if (goal->category() == category && goal->identifier() == identifier) {
            setActiveGoal(goal);
            return;
        }
    }
    qCritical() << "Could not select learning goal with ID " << identifier << ": not registered for this learner";
}

LearningGoal *Learner::activeGoal(Learner::Category categoryLearner) const
{
    // TODO:Qt5 change method parameter to LearningGoal::Category
    // workaround for Q_INVOKABLE access of enum
    LearningGoal::Category category = static_cast<LearningGoal::Category>(categoryLearner);
    if (!d->m_activeGoal.contains(category)) {
        qCWarning(LIBLEARNER_LOG) << "(Learner " << identifier() << ") No current learning goal set for category " << category << " : fall back to first in list";
        foreach (LearningGoal *goal, d->m_goals) {
            if (goal->category() == category) {
                return goal;
            }
        }
        qCWarning(LIBLEARNER_LOG) << "No learning goals of category " << category << " registered";
        return nullptr;
    }
    return d->m_activeGoal[category];
}
