/*
 *  Copyright 2018-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "drawertrainingactions.h"
#include "trainingaction.h"

#include <KLocalizedString>
#include <QDebug>
#include <QList>

DrawerTrainingActions::DrawerTrainingActions(QObject *parent)
    : QObject {parent}
    , m_defaultAction {new TrainingAction(i18n("Please select a course"), this)}
{
}

void DrawerTrainingActions::setSession(ISessionActions *session)
{
    if (session == m_session) {
        return;
    }
    if (m_session) {
        disconnect(m_session, &ISessionActions::courseChanged, this, &DrawerTrainingActions::actionsChanged);
        disconnect(m_session, &ISessionActions::actionsChanged, this, &DrawerTrainingActions::actionsChanged);
        disconnect(m_session, &ISessionActions::phraseChanged, this, &DrawerTrainingActions::triggerPhraseView);
    }

    m_session = session;
    connect(m_session, &ISessionActions::courseChanged, this, &DrawerTrainingActions::actionsChanged);
    connect(m_session, &ISessionActions::actionsChanged, this, &DrawerTrainingActions::actionsChanged);
    connect(m_session, &ISessionActions::phraseChanged, this, &DrawerTrainingActions::triggerPhraseView);

    emit sessionChanged();
    emit actionsChanged();
}

ISessionActions *DrawerTrainingActions::session() const
{
    return m_session;
}

QList<QObject *> DrawerTrainingActions::actions() const
{
    if (!m_session || m_session->trainingActions().isEmpty()) {
        QList<QObject *> list;
        list << qobject_cast<QObject *>(m_defaultAction);
        return list;
    }
    QList<QObject *> actions;
    const auto trainingActions = m_session->trainingActions();
    for (const auto &action : qAsConst(trainingActions)) {
        actions.append(qobject_cast<QObject *>(action));
    }
    return actions;
}
