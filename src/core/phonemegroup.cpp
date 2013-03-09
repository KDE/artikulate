/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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

#include "phonemegroup.h"
#include "phoneme.h"

#include <KDebug>

PhonemeGroup::PhonemeGroup(QObject *parent)
    : QObject(parent)
{
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

QList< Phoneme* > PhonemeGroup::phonemes() const
{
    return m_phonemes;
}

bool PhonemeGroup::contains(Phoneme *phoneme) const
{
    QList<Phoneme *>::ConstIterator iter = m_phonemes.constBegin();
    while (iter != m_phonemes.constEnd()) {
        if (QString::compare((*iter)->id(), phoneme->id()) == 0) {
            return true;
        }
        ++iter;
    }
    return false;
}

void PhonemeGroup::addPhoneme(Phoneme *phoneme)
{
    QList<Phoneme *>::ConstIterator iter = m_phonemes.constBegin();
    while (iter != m_phonemes.constEnd()) {
        if (QString::compare((*iter)->id(), phoneme->id()) == 0) {
            kWarning() << "Phoneme identifier already registered, aborting";
            return;
        }
        ++iter;
    }
    m_phonemes.append(phoneme);
}

Phoneme * PhonemeGroup::addPhoneme(const QString &identifier, const QString &title)
{
    Q_ASSERT(!identifier.isEmpty());

    // check that identifier is not used
    QList<Phoneme *>::ConstIterator iter = m_phonemes.constBegin();
    while (iter != m_phonemes.constEnd()) {
        if (QString::compare((*iter)->id(), identifier) == 0) {
            kWarning() << "Phoneme identifier " << identifier <<" already registered, aborting";
            return 0;
        }
        ++iter;
    }

    // create phoneme and add it
    Phoneme *newPhoneme = new Phoneme();
    newPhoneme->setId(identifier);
    newPhoneme->setTitle(title);
    addPhoneme(newPhoneme);

    return newPhoneme;
}

void PhonemeGroup::removePhoneme(Phoneme *phoneme)
{
    m_phonemes.removeOne(phoneme);
}
