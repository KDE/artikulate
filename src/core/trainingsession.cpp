/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "trainingsession.h"
#include "phrase.h"
#include "unit.h"

#include <QList>
#include <KDebug>
#include <KLocale>

TrainingSession::TrainingSession(QObject *parent)
    : QObject(parent)
{
}

TrainingSession::~TrainingSession()
{
    // nothing to do
}

QString TrainingSession::title() const
{
    return m_title;
}

void TrainingSession::setTitle(const QString &title)
{
    if (QString::compare(title, m_title) != 0) {
        m_title = title;
        emit titleChanged();
    }
}

Phrase * TrainingSession::currentPhrase()
{
    if (m_phraseListUntrained.isEmpty()) {
        return 0;
    }
    return m_phraseListUntrained.first().phrase;
}


Phrase::Type TrainingSession::currentType()
{
    return m_currentType;
}

void TrainingSession::next(TrainingSession::NextAction completeCurrent)
{
    TrainingPhrase currentPhrase = m_phraseListUntrained.first();
    switch(completeCurrent) {
    case Complete:
        ++currentPhrase.trainedGood;
        currentPhrase.isTrained = true;
        break;
    case RetryLater:
        ++currentPhrase.trainedBad;
        break;
    default:
        break;
    }
    m_phraseListTrained.append(currentPhrase);
    m_phraseListUntrained.removeFirst();
    emit currentPhraseChanged();
}

void TrainingSession::createFromUnit(Unit * unit)
{
    setTitle(unit->title());
    foreach(Phrase *phrase, unit->phraseList()) {
        TrainingPhrase newTrainingPhrase;
        newTrainingPhrase.isTrained = false;
        newTrainingPhrase.trainedBad = 0;
        newTrainingPhrase.trainedGood = 0;
        newTrainingPhrase.phrase = phrase;
        m_phraseListUntrained.append(newTrainingPhrase);
    }
    kDebug() << "create new training session with" << m_phraseListUntrained.length() << "elements";

    // set current phrase
    emit currentPhraseChanged();
}

