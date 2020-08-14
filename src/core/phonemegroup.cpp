/*
 *  SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "phonemegroup.h"
#include "phoneme.h"

#include "artikulate_debug.h"

PhonemeGroup::PhonemeGroup()
    : QObject(nullptr)
{
}

PhonemeGroup::~PhonemeGroup()
{
    for (auto phoneme : m_phonemes) {
        phoneme->deleteLater();
    }
    m_phonemes.clear();
}

QString PhonemeGroup::id() const
{
    return m_id;
}

void PhonemeGroup::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        emit idChanged();
    }
}

QString PhonemeGroup::title() const
{
    return m_title;
}

void PhonemeGroup::setTitle(const QString &title)
{
    if (QString::compare(title, m_title) != 0) {
        m_title = title;
        emit titleChanged();
    }
}

QString PhonemeGroup::description() const
{
    return m_description;
}

void PhonemeGroup::setDescription(const QString &description)
{
    m_description = description;
    emit descriptionChanged();
}

QVector<std::shared_ptr<Phoneme>> PhonemeGroup::phonemes() const
{
    return m_phonemes;
}

bool PhonemeGroup::contains(std::shared_ptr<Phoneme> phoneme) const
{
    for (auto testPhoneme : m_phonemes) {
        if (QString::compare(testPhoneme->id(), phoneme->id()) == 0) {
            return true;
        }
    }
    return false;
}

std::shared_ptr<Phoneme> PhonemeGroup::addPhoneme(std::unique_ptr<Phoneme> phoneme)
{
    std::shared_ptr<Phoneme> newPhoneme(std::move(phoneme));
    if (!contains(newPhoneme)) {
        m_phonemes.append(newPhoneme);
    } else {
        qCWarning(ARTIKULATE_LOG) << "Phoneme identifier already registered in group " << m_title << ", aborting";
    }
    return std::shared_ptr<Phoneme>();
}

std::shared_ptr<Phoneme> PhonemeGroup::addPhoneme(const QString &identifier, const QString &title)
{
    Q_ASSERT(!identifier.isEmpty());

    // check that identifier is not used
    for (auto phoneme : m_phonemes) {
        if (QString::compare(phoneme->id(), identifier) == 0) {
            qCWarning(ARTIKULATE_LOG) << "Phoneme identifier " << identifier << " already registered in group " << m_title << ", aborting";
            return std::shared_ptr<Phoneme>();
        }
    }

    std::unique_ptr<Phoneme> newPhoneme(new Phoneme);
    newPhoneme->setId(identifier);
    newPhoneme->setTitle(title);
    return addPhoneme(std::move(newPhoneme));
}
