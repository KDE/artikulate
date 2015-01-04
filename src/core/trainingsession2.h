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

#ifndef TRAININGSESSION2_H
#define TRAININGSESSION2_H

#include "artikulatecore_export.h"
#include "course.h"
#include "phrase.h"

class QString;
class Language;
class Course;
class Unit;
class PhonemeGroup;

/**
 * \class TrainingSession2
 * Global training session object, which eventually will replace TrainingSession.
 */
class ARTIKULATELIB_EXPORT TrainingSession2 : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Language *language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(Course *course READ course WRITE setCourse NOTIFY courseChanged)
    Q_PROPERTY(Unit *unit READ unit WRITE setUnit NOTIFY unitChanged)

public:
    explicit TrainingSession2(QObject *parent = 0);

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

Q_SIGNALS:
    void languageChanged();
    void courseChanged();
    void unitChanged();

private:
    Q_DISABLE_COPY(TrainingSession2)
    Language *m_language;
    Course *m_course;
    Unit *m_unit;
};

#endif
