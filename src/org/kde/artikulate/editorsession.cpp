/*
    SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "editorsession.h"
#include "artikulate_debug.h"
#include "core/iunit.h"
#include "core/phrase.h"
#include "core/resources/editablecourseresource.h"
#include "core/unit.h"

EditorSession::EditorSession(QObject *parent)
    : ISessionActions(parent)
{
    connect(this, &EditorSession::courseChanged, this, &EditorSession::skeletonModeChanged);
}

IEditableRepository *EditorSession::repository() const
{
    return m_repository;
}

void EditorSession::setRepository(IEditableRepository *repository)
{
    if (m_repository == repository) {
        return;
    }
    m_repository = repository;
    Q_EMIT repositoryChanged();
}

bool EditorSession::skeletonMode() const
{
    for (const auto &skeleton : m_repository->skeletons()) {
        if (skeleton->id() == m_course->id()) {
            return true;
        }
    }
    return false;
}

ILanguage *EditorSession::language() const
{
    if (m_course && m_course->language()) {
        return m_course->language().get();
    }
    return nullptr;
}

IEditableCourse *EditorSession::course() const
{
    return m_course;
}

void EditorSession::setCourse(IEditableCourse *course)
{
    if (m_course == course) {
        return;
    }
    m_course = course;

    if (m_course && m_course->units().count() > 0) {
        setActiveUnit(m_course->units().first().get());
    }
    Q_EMIT languageChanged();
    Q_EMIT courseChanged();
}

IUnit *EditorSession::activeUnit() const
{
    if (auto phrase = activePhrase()) {
        return phrase->unit().get();
    }
    return nullptr;
}

void EditorSession::setActiveUnit(IUnit *unit)
{
    // TODO also set unit without phrases
    if (!unit) {
        return;
    }
    for (const auto &phrase : unit->phrases()) {
        if (!phrase->sound().isEmpty()) {
            setActivePhrase(phrase.get());
            return;
        }
    }
}

void EditorSession::setActivePhrase(IPhrase *phrase)
{
    if (phrase == m_phrase) {
        return;
    }
    m_phrase = phrase;
    Q_EMIT phraseChanged();
}

IPhrase *EditorSession::activePhrase() const
{
    return m_phrase;
}

void EditorSession::switchToPreviousPhrase()
{
    if (!m_phrase) {
        return;
    }
    auto previous = m_phrase->previous();
    if (previous) {
        setActivePhrase(previous.get());
    }
}

void EditorSession::switchToNextPhrase()
{
    if (!m_phrase) {
        return;
    }
    auto next = m_phrase->next();
    if (next) {
        setActivePhrase(next.get());
    }
}

bool EditorSession::hasPreviousPhrase() const
{
    return m_phrase && m_phrase->previous() != nullptr;
}

bool EditorSession::hasNextPhrase() const
{
    return m_phrase && m_phrase->next() != nullptr;
}

void EditorSession::updateCourseFromSkeleton()
{
    if (!m_course) {
        qCritical() << "Not updating course from skeleton, no one set.";
        return;
    }
    m_repository->updateCourseFromSkeleton(m_course->self());
}
