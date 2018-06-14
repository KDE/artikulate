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

#include "drawertrainingactions.h"
#include "trainingaction.h"
#include "course.h"
#include "unit.h"

#include <QList>
#include <QDebug>

DrawerTrainingActions::DrawerTrainingActions(QObject* parent)
    : QObject(parent)
    , m_course(nullptr)
{
}

void DrawerTrainingActions::setCourse(Course *course)
{
    if (course == m_course) {
        return;
    }
    m_course = course;
    emit courseChanged(course);

    updateActions();
}

Course * DrawerTrainingActions::course() const
{
    return m_course;
}

void DrawerTrainingActions::setSession(TrainingSession *session)
{
    if (session == m_session) {
        return;
    }
    m_session = session;
    emit sessionChanged();
}

TrainingSession * DrawerTrainingActions::session() const
{
    return m_session;
}

QList<QObject*> DrawerTrainingActions::actions() const
{
    return m_actions;
}

void DrawerTrainingActions::updateActions()
{
    // cleanup
    for (const auto &action : m_actions) {
        action->deleteLater();
    }
    m_actions.clear();

    if (!m_course) {
        return;
    }

    for (const auto &unit : m_course->unitList()) {
        auto action = new TrainingAction(unit->title());
        m_actions.append(action);
        for (const auto &phrase : unit->phraseList()) {
            action->appendChild(new TrainingAction(phrase, this, unit));
        }
    }
    emit actionsChanged();
}
