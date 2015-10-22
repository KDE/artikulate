/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
#include "course.h"
#include "phrase.h"

class QString;
class Language;
class Course;
class Unit;
class PhonemeGroup;

/**
 * \class TrainingSession
 */
class ARTIKULATECORE_EXPORT TrainingSession : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Language *language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(Course *course READ course WRITE setCourse NOTIFY courseChanged)
    Q_PROPERTY(Unit *unit READ unit WRITE setUnit NOTIFY unitChanged)
    Q_PROPERTY(Phrase *phrase READ phrase WRITE setPhrase NOTIFY phraseChanged)
    Q_PROPERTY(bool hasNextPhrase READ hasNextPhrase NOTIFY phraseChanged)

public:
    explicit TrainingSession(QObject *parent = nullptr);

    Language * language() const;
    void setLanguage(Language *language);
    Course * course() const;
    void setCourse(Course *course);
    Unit * unit() const;
    void setUnit(Unit *unit);
    PhonemeGroup * phonemeGroup() const;
    void setPhonemeGroup(PhonemeGroup *phonemeGroup);
    Phrase::Type phraseType() const;
    void setPhraseType(Phrase::Type type);
    Phrase * phrase() const;
    void setPhrase(Phrase *phrase);
    bool hasPreviousPhrase() const;
    bool hasNextPhrase() const;
    Q_INVOKABLE void showNextPhrase();
    Q_INVOKABLE void skipPhrase();

Q_SIGNALS:
    void languageChanged();
    void courseChanged();
    void unitChanged();
    void phraseChanged();

private:;
    Q_DISABLE_COPY(TrainingSession)
    Phrase * nextPhrase() const;
    Language *m_language;
    Course *m_course;
    Unit *m_unit;
    Phrase *m_phrase;
};

#endif
