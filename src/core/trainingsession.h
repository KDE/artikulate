/*
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *  Copyright 2013       Oindrila Gupta <oindrila.gupta92@gmail.com>
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

#ifndef TRAININGSESSION_H
#define TRAININGSESSION_H

#include "artikulatecore_export.h"
#include "phrase.h"
#include <QObject>
#include <QList>

class QString;
class Phrase;
class Unit;

class ARTIKULATELIB_EXPORT TrainingSession : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(bool isFinished READ isFinished NOTIFY finished)
    Q_PROPERTY(Phrase *currentPhrase READ currentPhrase NOTIFY currentPhraseChanged)
    Q_PROPERTY(Phrase::Type currentType READ currentType NOTIFY currentTypeChanged)
    Q_PROPERTY(int progressTypeWord READ progressTypeWord NOTIFY progressChanged) //!< value between 0 and 100 that gives percentage value
    Q_PROPERTY(int progressTypeExpression READ progressTypeExpression NOTIFY progressChanged) //!< value between 0 and 100 that gives percentage value
    Q_PROPERTY(int progressTypeSentence READ progressTypeSentence NOTIFY progressChanged) //!< value between 0 and 100 that gives percentage value
    Q_PROPERTY(int progressTypeParagraph READ progressTypeParagraph NOTIFY progressChanged) //!< value between 0 and 100 that gives percentage value

    struct TrainingPhrase {
        Phrase *phrase;
        int tries;
        bool isTrained;
    };

public:
    Q_ENUMS(NextAction)
    enum NextAction {
        Complete,
        Incomplete
    };

    // TODO workaround for QT-BUG-26415, fixed in Qt 5.0
    // we must simulate the Phrase::Type enum in TrainingSession to be able to allow its usage
    // as parameter for Q_INVOKABLE method
    // can be removed with Qt 5.0 migration
    Q_ENUMS(Type)
    enum Type {
        Word = Phrase::Word,
        Expression = Phrase::Expression,
        Sentence = Phrase::Sentence,
        Paragraph = Phrase::Paragraph
    };

    explicit TrainingSession(QObject *parent = 0);
    ~TrainingSession();

    QString title() const;
    void setTitle(const QString &title);
    Phrase * currentPhrase() const;
    Phrase::Type currentType() const;
    QList<Phrase *> phraseList() const;
    void addPhrase(Phrase *phrase);
    bool isFinished() const;

    /**
     * Create training session from \p unit.
     */
    Q_INVOKABLE void createFromUnit(Unit *unit);

    /**
     * Proceeds with next phrase to train.
     * Set statistics value for current phrase to \p completeCurrent;
     */
    Q_INVOKABLE void next(NextAction completeCurrent);
    Q_INVOKABLE void setPhraseType(const QString &newType);
    Q_INVOKABLE void jumpToPhrase(Phrase *phrase);
    Q_INVOKABLE void stop();

    // TODO workaround for QT-BUG-26415, remove after migration to Qt-5
    Q_INVOKABLE int numberPhrasesGroupedByTries(TrainingSession::Type type, int neededTries) const;
    Q_INVOKABLE int numberPhrases(TrainingSession::Type type) const;

    Q_INVOKABLE int numberPhrasesGroupedByTries(Phrase::Type type, int neededTries) const;
    Q_INVOKABLE int numberPhrases(Phrase::Type type) const;

    /**
     * \return the maximum number of phrases having the same number of tries
     */
    int maximumPhrasesPerTry() const;

    /**
     * \return the maximum number of tries needed for any phrase
     */
    Q_INVOKABLE int maximumTries() const;

    int progressTypeWord() const;
    int progressTypeExpression() const;
    int progressTypeSentence() const;
    int progressTypeParagraph() const;

signals:
    void titleChanged();
    void currentPhraseChanged();
    void currentTypeChanged();
    void progressChanged();
    void finished();

private:
    Q_DISABLE_COPY(TrainingSession)
    QString m_title;
    Phrase::Type m_currentType;
    QMap< Phrase::Type, QList<TrainingPhrase> > m_phraseListUntrained;
    QMap< Phrase::Type, QList<TrainingPhrase> > m_phraseListTrained;
};

#endif // TRAININGSESSION_H
