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

#include "phrase.h"


#include <phonon/audiooutput.h>

#include <KDebug>

Phrase::Phrase(QObject *parent)
    : QObject(parent)
    , m_sound(new Phonon::MediaObject(this))
    , m_userSound(new Phonon::MediaObject(this))
{
    Phonon::createPath(m_sound, new Phonon::AudioOutput(Phonon::MusicCategory, this));
    Phonon::createPath(m_userSound, new Phonon::AudioOutput(Phonon::MusicCategory, this));
    connect(m_sound, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SIGNAL(playbackSoundStateChanged()));
    connect(m_userSound, SIGNAL(stateChanged(Phonon::State,Phonon::State)),
            this, SIGNAL(playbackUserSoundStateChanged()));
}

Phrase::~Phrase()
{
    delete m_sound;
    delete m_userSound;
}

QString Phrase::id() const
{
    return m_id;
}

void Phrase::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        emit idChanged();
    }
}

QString Phrase::text() const
{
    return m_text;
}

void Phrase::setText(const QString &text)
{
    if (QString::compare(text, m_text) != 0) {
        m_text = text;
        emit textChanged();
    }
}

Phrase::Type Phrase::type() const
{
    return m_type;
}

QString Phrase::typeString() const
{
    switch(m_type) {
    case Word:
        return "word";
    case Expression:
        return "expression";
    case Sentence:
        return "sentence";
    case Paragraph:
        return "paragraph";
    default:
        return "ERROR_UNKNOWN_TYPE";
    }
}

void Phrase::setType(Phrase::Type type)
{
    m_type = type;
}

void Phrase::setType(const QString &typeString)
{
    if (typeString == "word") {
        m_type = Word;
        return;
    }
    if (typeString == "expression") {
        m_type = Expression;
        return;
    }
    if (typeString == "sentence") {
        m_type = Sentence;
        return;
    }
    if (typeString == "paragraph") {
        m_type = Paragraph;
        return;
    }
    kWarning() << "Cannot set type from unknown identifier, aborting";
    return;
}

KUrl Phrase::sound() const
{
    return m_sound->currentSource().url();
}

void Phrase::setSound(const KUrl &soundFile)
{
    if (!soundFile.isValid() || soundFile.isEmpty()) {
        kWarning() << "Not setting empty sound file path.";
        return;
    }
    m_sound->setCurrentSource(soundFile);
    emit soundChanged();
}

void Phrase::playbackSound()
{
    kDebug() << "Playing authentic sound";

    m_sound->play();
}

void Phrase::playbackUserSound()
{
    kDebug() << "Playing user recorded sound";

    m_userSound->setCurrentSource(m_userSoundFile);
    m_userSound->play();
    m_userSound->currentSource();
}

Phrase::PlaybackState Phrase::playbackSoundState() const
{
    switch (m_sound->state())
    {
    case Phonon::PlayingState:
        return Phrase::PlayingState;
    case Phonon::PausedState:
        return Phrase::PausedState;
    default:
        return Phrase::StoppedState;
    }
}

Phrase::PlaybackState Phrase::playbackUserSoundState() const
{
    switch (m_userSound->state())
    {
    case Phonon::PlayingState:
        return Phrase::PlayingState;
    case Phonon::PausedState:
        return Phrase::PausedState;
    default:
        return Phrase::StoppedState;
    }
}

void Phrase::stopSound()
{
    m_sound->stop();
}

void Phrase::stopUserSound()
{
    m_userSound->stop();
}

bool Phrase::isUserSound() const
{
    return m_userSoundFile.isValid() && !m_userSoundFile.isEmpty();
}

QList<Tag *> Phrase::tags() const
{
    return m_prononciationTags;
}

void Phrase::addTag(Tag *tag)
{
    if (!m_prononciationTags.contains(tag)) {
        m_prononciationTags.append(tag);
    }
}
