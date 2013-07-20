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
    m_phraseListTrained.insert(Phrase::Word, QList<TrainingPhrase>());
    m_phraseListTrained.insert(Phrase::Expression, QList<TrainingPhrase>());
    m_phraseListTrained.insert(Phrase::Sentence, QList<TrainingPhrase>());
    m_phraseListTrained.insert(Phrase::Paragraph, QList<TrainingPhrase>());

    m_phraseListUntrained.insert(Phrase::Word, QList<TrainingPhrase>());
    m_phraseListUntrained.insert(Phrase::Expression, QList<TrainingPhrase>());
    m_phraseListUntrained.insert(Phrase::Sentence, QList<TrainingPhrase>());
    m_phraseListUntrained.insert(Phrase::Paragraph, QList<TrainingPhrase>());
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

Phrase * TrainingSession::currentPhrase() const
{
    if (m_phraseListUntrained.value(m_currentType).isEmpty()) {
        return 0;
    }
    return m_phraseListUntrained.value(m_currentType).first().phrase;
}


Phrase::Type TrainingSession::currentType() const
{
    return m_currentType;
}

int TrainingSession::progressTypeWord() const
{
    if (m_phraseListUntrained.value(Phrase::Word).isEmpty()) {
        return 100;
    }
    int totalNumber = m_phraseListTrained.value(Phrase::Word).length() + m_phraseListUntrained.value(Phrase::Word).length();
    return 100 * m_phraseListTrained.value(Phrase::Word).length() / totalNumber;
}

int TrainingSession::progressTypeExpression() const
{
    if (m_phraseListUntrained.value(Phrase::Expression).isEmpty()) {
        return 100;
    }
    int totalNumber = m_phraseListTrained.value(Phrase::Expression).length() + m_phraseListUntrained.value(Phrase::Expression).length();
    return 100 * m_phraseListTrained.value(Phrase::Expression).length() / totalNumber;
}

int TrainingSession::progressTypeSentence() const
{
    if (m_phraseListUntrained.value(Phrase::Sentence).isEmpty()) {
        return 100;
    }
    int totalNumber = m_phraseListTrained.value(Phrase::Sentence).length() + m_phraseListUntrained.value(Phrase::Sentence).length();
    return 100 * m_phraseListTrained.value(Phrase::Sentence).length() / totalNumber;
}

int TrainingSession::progressTypeParagraph() const
{
    if (m_phraseListUntrained.value(Phrase::Paragraph).isEmpty()) {
        return 100;
    }
    int totalNumber = m_phraseListTrained.value(Phrase::Paragraph).length() + m_phraseListUntrained.value(Phrase::Paragraph).length();
    return 100 * m_phraseListTrained.value(Phrase::Paragraph).length() / totalNumber;
}

void TrainingSession::next(TrainingSession::NextAction completeCurrent)
{
    TrainingPhrase currentPhrase = m_phraseListUntrained.value(m_currentType).first();
    switch(completeCurrent) {
    case Complete:
        ++currentPhrase.trainedGood;
        currentPhrase.isTrained = true;
        break;
    case Incomplete:
        ++currentPhrase.trainedBad;
        break;
    default:
        break;
    }

    // if completed, then put to completed list
    if (completeCurrent == Complete) {
        m_phraseListTrained[currentPhrase.phrase->type()].append(currentPhrase);
        m_phraseListUntrained[currentPhrase.phrase->type()].removeFirst();
    }
    // else put to end of list for later try
    else {
        m_phraseListUntrained[currentPhrase.phrase->type()].move(0,m_phraseListUntrained[currentPhrase.phrase->type()].size()-1);
    }

    // switch type when all phrases in current set are trained
    // use loop here, since types can be empty
    while (m_phraseListUntrained.value(m_currentType).isEmpty()) {
        if (m_currentType == Phrase::Word) {
            m_currentType = Phrase::Expression;
            break;
        }
        if (m_currentType == Phrase::Expression) {
            m_currentType = Phrase::Sentence;
            break;
        }
        if (m_currentType == Phrase::Sentence) {
            m_currentType = Phrase::Paragraph;
            break;
        }
        if (m_currentType == Phrase::Paragraph) {
            break;
        }
        emit currentTypeChanged();
    }

    emit currentPhraseChanged();
    emit progressChanged();
}

void TrainingSession::createFromUnit(Unit * unit)
{
    m_phraseListTrained[Phrase::Word].clear();
    m_phraseListTrained[Phrase::Expression].clear();
    m_phraseListTrained[Phrase::Sentence].clear();
    m_phraseListTrained[Phrase::Paragraph].clear();
    m_phraseListUntrained[Phrase::Word].clear();
    m_phraseListUntrained[Phrase::Expression].clear();
    m_phraseListUntrained[Phrase::Sentence].clear();
    m_phraseListUntrained[Phrase::Paragraph].clear();

    setTitle(unit->title());
    foreach(Phrase *phrase, unit->phraseList()) {
        TrainingPhrase newTrainingPhrase;
        newTrainingPhrase.isTrained = false;
        newTrainingPhrase.trainedBad = 0;
        newTrainingPhrase.trainedGood = 0;
        newTrainingPhrase.phrase = phrase;
        m_phraseListUntrained[phrase->type()].append(newTrainingPhrase);
    }

    // set current phrase
    m_currentType = Phrase::Word;
    emit currentPhraseChanged();
    emit progressChanged();
}

