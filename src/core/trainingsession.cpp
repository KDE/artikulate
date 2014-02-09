/*
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *  Copyright 2013       Oindrila Gupta <oindrila.gupta92@gmail.com>
 *  Copyright 2013       Samikshan Bairagya <samikshan@gmail.com>
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
#include <QHash>
#include <KDebug>
#include <KLocale>

TrainingSession::TrainingSession(QObject *parent)
    : QObject(parent)
    , m_title(QString())
    , m_currentType(Phrase::AllTypes)
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
    return 100 * m_phraseListTrained.value(Phrase::Word).length() / numberPhrases(Phrase::Word);
}

int TrainingSession::progressTypeExpression() const
{
    if (m_phraseListUntrained.value(Phrase::Expression).isEmpty()) {
        return 100;
    }
    return 100 * m_phraseListTrained.value(Phrase::Expression).length() / numberPhrases(Phrase::Expression);
}

int TrainingSession::progressTypeSentence() const
{
    if (m_phraseListUntrained.value(Phrase::Sentence).isEmpty()) {
        return 100;
    }
    return 100 * m_phraseListTrained.value(Phrase::Sentence).length() / numberPhrases(Phrase::Sentence);
}

int TrainingSession::progressTypeParagraph() const
{
    if (m_phraseListUntrained.value(Phrase::Paragraph).isEmpty()) {
        return 100;
    }
    return 100 * m_phraseListTrained.value(Phrase::Paragraph).length() / numberPhrases(Phrase::Paragraph);
}

bool TrainingSession::isFinished() const
{
    if (m_phraseListUntrained[Phrase::Word].isEmpty()
        && m_phraseListUntrained[Phrase::Expression].isEmpty()
        && m_phraseListUntrained[Phrase::Sentence].isEmpty()
        && m_phraseListUntrained[Phrase::Paragraph].isEmpty())
    {
        return true;
    }
    return false;
}

void TrainingSession::setPhraseType(const QString &newType)
{
    if (newType=="word" && not m_phraseListUntrained[Phrase::Word].isEmpty()) {
        m_currentType = Phrase::Word;
    }
    if (newType=="expression" && not m_phraseListUntrained[Phrase::Expression].isEmpty()) {
        m_currentType = Phrase::Expression;
    }
    if (newType=="sentence"&& not m_phraseListUntrained[Phrase::Sentence].isEmpty()) {
        m_currentType = Phrase::Sentence;
    }
    if (newType=="paragraph" && not m_phraseListUntrained[Phrase::Paragraph].isEmpty()) {
        m_currentType = Phrase::Paragraph;
    }
    emit currentTypeChanged();
    emit currentPhraseChanged();
}

void TrainingSession::jumpToPhrase(Phrase* phrase)
{
    bool isPresent = false;
    TrainingPhrase currentPhrase;
    int index = 0;
    foreach (const TrainingPhrase &trPhrase, m_phraseListUntrained.value(phrase->type())) {
        if (trPhrase.phrase->id() == phrase->id()) {
            isPresent = true;
            currentPhrase = trPhrase;
            break;
        }
        else {
            index++;
        }
    }

    if (!isPresent) { //phrase is not in list of untrained phrases
        return;
    }

    //Reorder untrained list for the phrase type
    m_currentType = phrase->type();
    QList<TrainingSession::TrainingPhrase> phraseList;
    for (int i = index; i < m_phraseListUntrained.value(m_currentType).length(); i++) {
        phraseList.append(m_phraseListUntrained.value(m_currentType).at(i));
    }
    for (int i = 0; i < index; i++) {
        phraseList.append(m_phraseListUntrained.value(m_currentType).at(i));
    }
    m_phraseListUntrained[m_currentType] = phraseList;

    emit currentTypeChanged();
    emit currentPhraseChanged();
    emit progressChanged();
}

void TrainingSession::next(TrainingSession::NextAction completeCurrent)
{
    TrainingPhrase &currentPhrase = const_cast<TrainingPhrase&>(m_phraseListUntrained.value(m_currentType).first());
    switch(completeCurrent) {
    case Complete:
        ++currentPhrase.tries;
        currentPhrase.isTrained = true;
        currentPhrase.phrase->setTrainingState(Phrase::Trained);
        break;
    case Incomplete:
        ++currentPhrase.tries;
        break;
    default:
        break;
    }

    // if completed, then put to completed list and check for maximum number of tries
    if (completeCurrent == Complete) {
        m_phraseListTrained[currentPhrase.phrase->type()].append(currentPhrase);
        m_phraseListUntrained[currentPhrase.phrase->type()].removeFirst();
    }
    // else put to end of list for later try
    else {
        m_phraseListUntrained[currentPhrase.phrase->type()].move(0,m_phraseListUntrained[currentPhrase.phrase->type()].size()-1);
    }

    // check for course completion
    if (m_phraseListUntrained.value(Phrase::Word).isEmpty() &&
        m_phraseListUntrained.value(Phrase::Expression).isEmpty() &&
        m_phraseListUntrained.value(Phrase::Sentence).isEmpty() &&
        m_phraseListUntrained.value(Phrase::Paragraph).isEmpty()
    ) {
       emit finished();
    }

    // switch type when all phrases in current set are trained
    // use loop here, since types can be empty
    while (m_phraseListUntrained.value(m_currentType).isEmpty()) {
        if (m_currentType == Phrase::Word) {
            if (m_phraseListUntrained.value(Phrase::Expression).isEmpty()) {
                if (m_phraseListUntrained.value(Phrase::Sentence).isEmpty()) {
                    m_currentType = Phrase::Paragraph;
                    break;
                }
                m_currentType = Phrase::Sentence;
                break;
            }
            m_currentType = Phrase::Expression;
            break;
        }

        if (m_currentType == Phrase::Expression) {
            if (m_phraseListUntrained.value(Phrase::Sentence).isEmpty()) {
                if (m_phraseListUntrained.value(Phrase::Paragraph).isEmpty()) {
                    m_currentType = Phrase::Word;
                    break;
                }
                m_currentType = Phrase::Paragraph;
                break;
            }
            m_currentType = Phrase::Sentence;
            break;
        }
        if (m_currentType == Phrase::Sentence) {
            if (m_phraseListUntrained.value(Phrase::Paragraph).isEmpty()) {
                if (m_phraseListUntrained.value(Phrase::Word).isEmpty()) {
                    m_currentType = Phrase::Expression;
                    break;
                }
                m_currentType = Phrase::Word;
                break;
            }
            m_currentType = Phrase::Paragraph;
            break;
        }
        if (m_currentType == Phrase::Paragraph) {
            if (m_phraseListUntrained.value(Phrase::Word).isEmpty()) {
                if (m_phraseListUntrained.value(Phrase::Expression).isEmpty()) {
                    m_currentType = Phrase::Sentence;
                    break;
                }
                m_currentType = Phrase::Expression;
                break;
            }
            m_currentType = Phrase::Word;
            break;
        }
    }
    emit currentTypeChanged();
    emit currentPhraseChanged();
    emit progressChanged();
}

void TrainingSession::stop()
{
    m_phraseListUntrained[Phrase::Word].clear();
    m_phraseListUntrained[Phrase::Expression].clear();
    m_phraseListUntrained[Phrase::Sentence].clear();
    m_phraseListUntrained[Phrase::Paragraph].clear();
    emit finished();
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
        // reset every training status
        phrase->setTrainingState(Phrase::Untrained);

        if (phrase->sound().isEmpty()
            || phrase->isExcluded() == true)
        {
            continue;
        }
        TrainingPhrase newTrainingPhrase;
        newTrainingPhrase.isTrained = false;
        newTrainingPhrase.tries = 0;
        newTrainingPhrase.phrase = phrase;
        m_phraseListUntrained[phrase->type()].append(newTrainingPhrase);
    }

        m_currentType = Phrase::Word;
        emit currentPhraseChanged();
        emit progressChanged();
        emit currentTypeChanged();

        emit finished(); //TODO work around for now, since we allow access to empty units at trainer
}

int TrainingSession::numberPhrasesGroupedByTries(TrainingSession::Type type, int neededTries) const
{
    return numberPhrasesGroupedByTries(static_cast<Phrase::Type>(type), neededTries);
}

int TrainingSession::numberPhrasesGroupedByTries(Phrase::Type type, int neededTries) const
{
    int numberOfPhrasesForTries = 0;
    foreach (const TrainingPhrase &phrase, m_phraseListTrained[type]) {
        if (phrase.tries == neededTries) {
            ++numberOfPhrasesForTries;
        }
    }
    return numberOfPhrasesForTries;
}

int TrainingSession::numberPhrases(TrainingSession::Type type) const
{
    return numberPhrases(static_cast<Phrase::Type>(type));
}

int TrainingSession::numberPhrases(Phrase::Type type) const
{
    return m_phraseListTrained.value(type).length() + m_phraseListUntrained.value(type).length();
}

int TrainingSession::maximumPhrasesPerTry() const
{
    int maxTries = 0;
    QHash<int, int> numberPhrases;
    foreach (const TrainingPhrase &phrase, m_phraseListTrained[Phrase::Word]) {
        if (!numberPhrases.contains(phrase.tries)) {
            numberPhrases.insert(phrase.tries, 1);
        } else {
            numberPhrases[phrase.tries] = numberPhrases[phrase.tries] + 1;
        }
        if (maxTries < numberPhrases[phrase.tries]) {
            maxTries = numberPhrases[phrase.tries];
        }
    }
    return maxTries;
}

int TrainingSession::maximumTries() const
{
    int maxTries = 1;
    foreach (const TrainingPhrase &phrase, m_phraseListTrained[Phrase::Word]) {
        if (maxTries < phrase.tries) {
            maxTries = phrase.tries;
        }
    }
    foreach (const TrainingPhrase &phrase, m_phraseListTrained[Phrase::Expression]) {
        if (maxTries < phrase.tries) {
            maxTries = phrase.tries;
        }
    }
    foreach (const TrainingPhrase &phrase, m_phraseListTrained[Phrase::Sentence]) {
        if (maxTries < phrase.tries) {
            maxTries = phrase.tries;
        }
    }
    foreach (const TrainingPhrase &phrase, m_phraseListTrained[Phrase::Paragraph]) {
        if (maxTries < phrase.tries) {
            maxTries = phrase.tries;
        }
    }
    // return bad tries plus one good try
    return maxTries;
}
