/*
 *  Copyright 2018  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License or (at your option) version 3 or any later version
 *  accepted by the membership of KDE e.V. (or its successor approved
 *  by the membership of KDE e.V.), which shall act as a proxy
 *  defined in Section 14 of version 3 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "trainingaction.h"
#include "drawertrainingactions.h"
#include "trainingsession.h"

TrainingAction::TrainingAction(QObject *parent)
    : QObject(parent)
    , m_text(QString())
    , m_iconName(QString()) //TODO "rating-unrated" vs. "rating"
{
}

TrainingAction::TrainingAction(const QString &text, QObject *parent)
    : QObject(parent)
    , m_text(text)
    , m_iconName(QString()) //TODO "rating-unrated" vs. "rating"
{
}

TrainingAction::TrainingAction(Phrase *phrase, DrawerTrainingActions *drawerTrainingActions, QObject* parent)
    : QObject(parent)
    , m_phrase(phrase)
    , m_drawerTrainingActions(drawerTrainingActions)
{
    if (m_phrase) {
        m_text = phrase->text();
    }
}

void TrainingAction::appendChild(QObject* child)
{
    m_children.append(child);
    emit childrenChanged();
}

void TrainingAction::trigger()
{
    if (m_phrase && m_drawerTrainingActions && m_drawerTrainingActions->session()) {
        m_drawerTrainingActions->session()->setPhrase(m_phrase);
        emit m_drawerTrainingActions->triggerTrainingView();
    }
}

bool TrainingAction::enabled() const
{
    return m_enabled;
}

void TrainingAction::setEnabled(bool enabled)
{
    if (enabled == m_enabled) {
        return;
    }
    m_enabled = enabled;
    emit enabledChanged(m_enabled);
}


