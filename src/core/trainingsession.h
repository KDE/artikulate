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
    Q_PROPERTY(Phrase *currentPhrase READ currentPhrase NOTIFY currentPhraseChanged)
    Q_PROPERTY(Phrase::Type currentType READ currentType NOTIFY currentTypeChanged)

    struct TrainingPhrase {
        Phrase *phrase;
        int trainedBad;
        int trainedGood;
        bool isTrained;
    };
    enum NextAction {
        Complete,
        RetryLater,
        StepOver
    };

public:
    explicit TrainingSession(QObject *parent = 0);
    ~TrainingSession();

    QString title() const;
    void setTitle(const QString &title);
    Phrase * currentPhrase();
    Phrase::Type currentType();
    QList<Phrase *> phraseList() const;
    void addPhrase(Phrase *phrase);

    /**
     * Create training session from \p unit.
     */
    void createFromUnit(Unit *unit);

    /**
     * Proceeds with next phrase to train.
     * Set statistics value for current phrase to \p completeCurrent;
     */
    void next(NextAction completeCurrent);

signals:
    void titleChanged();
    void currentPhraseChanged();
    void currentTypeChanged();

private:
    Q_DISABLE_COPY(TrainingSession)
    QString m_title;
    Phrase::Type m_currentType;
    QList<TrainingPhrase> m_phraseListUntrained;
    QList<TrainingPhrase> m_phraseListTrained;
};

#endif // TRAININGSESSION_H
