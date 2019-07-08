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

#ifndef EDITORSESSION_H
#define EDITORSESSION_H

#include "artikulatecore_export.h"
#include "phrase.h"

class ILanguage;
class IEditableCourse;
class Unit;
class SkeletonResource;
class IEditableRepository;

/**
 * \class EditorSession
 *
 * An object of this class is used to set the current state of the editor. By this, we put all logic
 * how language, skeleton and course fit to each other into this class. The main concept is that
 * we have to fundamentally different workflows that both are modeled in this class:
 *
 * 1. Skeleton based workflow
 * - a skeleton is selected
 * - every language is available, since eventually the course should be available in every language
 * - for every language, there is at most one course (there is none only in case it is not created yet)
 * - adding new units or phrases is only possible in the skeleton course
 * - every course can update/sync with the skeleton
 *
 * 2. Course based workflow
 * - there is no skeleton from which the course is derived
 * - the base is a language that is selected first
 * - for a language there can be none to arbitrarily many courses
 *
 * The main switch is \c EditorSession::setSkeletonMode(bool)
 */
class ARTIKULATECORE_EXPORT EditorSession : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool skeletonMode READ skeletonMode NOTIFY skeletonModeChanged)
    Q_PROPERTY(bool editSkeleton READ isEditSkeleton WRITE setEditSkeleton NOTIFY editSkeletonChanged)
    Q_PROPERTY(IEditableCourse *skeleton READ skeleton WRITE setSkeleton NOTIFY skeletonChanged)
    Q_PROPERTY(IEditableCourse *course READ course WRITE setCourse NOTIFY courseChanged)

    // editor elements depending on curently selected mode, skeleton and course
    /**
     * @brief the displayed course (skeleton or course) depending on the user selection
     */
    Q_PROPERTY(IEditableCourse *displayedCourse READ displayedCourse NOTIFY displayedCourseChanged)
    Q_PROPERTY(ILanguage *language READ language NOTIFY languageChanged)
    Q_PROPERTY(Unit *unit READ unit WRITE setUnit NOTIFY unitChanged)
    Q_PROPERTY(Phrase *phrase READ phrase WRITE setPhrase NOTIFY phraseChanged)
    Q_PROPERTY(bool hasNextPhrase READ hasNextPhrase NOTIFY phraseChanged)
    Q_PROPERTY(bool hasPreviousPhrase READ hasPreviousPhrase NOTIFY phraseChanged)

public:
    explicit EditorSession(QObject *parent = nullptr);

    void setRepository(IEditableRepository *repository);
    bool skeletonMode() const;
    void setEditSkeleton(bool enabled=true);
    bool isEditSkeleton() const;
    IEditableCourse * skeleton() const;
    void setSkeleton(IEditableCourse *skeleton);
    ILanguage * language() const;
    IEditableCourse * course() const;
    void setCourse(IEditableCourse *course);
    /**
     * @brief Open course resource by specifying the language
     * @param language the target language
     */
    Q_INVOKABLE void setCourseByLanguage(ILanguage *language);
    IEditableCourse * displayedCourse() const;
    Q_DECL_DEPRECATED Unit * unit() const;
    Unit * activeUnit() const;
    void setUnit(Unit *unit);
    Q_DECL_DEPRECATED Phrase * phrase() const;
    Phrase * activePhrase() const;
    void setPhrase(Phrase *phrase);
    Phrase::Type phraseType() const;
    void setPhraseType(Phrase::Type type);
    bool hasPreviousPhrase() const;
    bool hasNextPhrase() const;
    Q_INVOKABLE void switchToPreviousPhrase();
    Q_INVOKABLE void switchToNextPhrase();
    Q_INVOKABLE void updateCourseFromSkeleton();

private:
    Phrase * nextPhrase() const;
    Phrase * previousPhrase() const;

private Q_SLOTS:
    void updateDisplayedUnit();

Q_SIGNALS:
    void editSkeletonChanged();
    void skeletonModeChanged();
    void skeletonChanged();
    void languageChanged();
    void courseChanged();
    void displayedCourseChanged();
    void unitChanged();
    void phraseChanged();

private:
    Q_DISABLE_COPY(EditorSession)
    IEditableRepository * m_repository{ nullptr };
    bool m_editSkeleton{ false };
    IEditableCourse *m_skeleton{ nullptr };
    ILanguage *m_language{ nullptr };
    IEditableCourse *m_course{ nullptr };
    Unit *m_unit{ nullptr };
    Phrase *m_phrase{ nullptr };
};

#endif
