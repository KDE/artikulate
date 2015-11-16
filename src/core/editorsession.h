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
#include "course.h"
#include "phrase.h"

class QString;
class Skeleton;
class Language;
class Course;
class Unit;
class ResourceManager;

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
    Q_PROPERTY(bool skeletonMode READ skeletonMode WRITE setSkeletonMode NOTIFY skeletonModeChanged)
    Q_PROPERTY(bool editSkeleton READ isEditSkeleton WRITE setEditSkeleton NOTIFY editSkeletonChanged)
    Q_PROPERTY(Skeleton *skeleton READ skeleton WRITE setSkeleton NOTIFY skeletonChanged)
    Q_PROPERTY(Language *language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(Course *course READ course WRITE setCourse NOTIFY courseChanged)
    Q_PROPERTY(Unit *unit READ unit WRITE setUnit NOTIFY unitChanged)
    Q_PROPERTY(Phrase *phrase READ phrase WRITE setPhrase NOTIFY phraseChanged)
    Q_PROPERTY(bool hasNextPhrase READ hasNextPhrase NOTIFY phraseChanged)
    Q_PROPERTY(bool hasPreviousPhrase READ hasPreviousPhrase NOTIFY phraseChanged)

public:
    explicit EditorSession(QObject *parent = nullptr);

    void setResourceManager(ResourceManager *manager);
    void setSkeletonMode(bool enabled=true);
    bool skeletonMode() const;
    void setEditSkeleton(bool enabled=true);
    bool isEditSkeleton() const;
    Skeleton * skeleton() const;
    void setSkeleton(Skeleton *skeleton);
    Language * language() const;
    void setLanguage(Language *language);
    Course * course() const;
    void setCourse(Course *course);
    Unit * unit() const;
    void setUnit(Unit *unit);
    Phrase * phrase() const;
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

Q_SIGNALS:
    void editSkeletonChanged();
    void skeletonModeChanged();
    void skeletonChanged();
    void languageChanged();
    void courseChanged();
    void unitChanged();
    void phraseChanged();

private:
    Q_DISABLE_COPY(EditorSession)
    ResourceManager * m_resourceManager;
    bool m_skeletonMode;
    bool m_editSkeleton;
    Skeleton *m_skeleton;
    Language *m_language;
    Course *m_course;
    Course *m_tmpCourseWhileSkeletonEditing;
    Unit *m_unit;
    Phrase *m_phrase;
};

#endif
