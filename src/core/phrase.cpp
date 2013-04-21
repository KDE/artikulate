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
#include "capturedevicecontroller.h"

#include <QMediaPlayer>

#include <KDebug>
#include <KTemporaryFile>

Phrase::Phrase(QObject *parent)
    : QObject(parent)
    , m_audioOutput(new QMediaPlayer)
{
    m_currentPlayback = None;

    // register recording file
    m_userSoundFile.setSuffix(".ogg");
    m_userSoundFile.open();

    // TODO too many emits
    connect(m_audioOutput, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SIGNAL(playbackSoundStateChanged()));
    connect(m_audioOutput, SIGNAL(stateChanged(QMediaPlayer::State)),
            this, SIGNAL(playbackUserSoundStateChanged()));
}

Phrase::~Phrase()
{
    delete m_audioOutput;

    // clear resources
    m_userSoundFile.close();
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

QString Phrase::foreignId() const
{
    return m_foreignId;
}

void Phrase::setForeignId(const QString &id)
{
    m_foreignId = id;
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
    if (m_type == type) {
        return;
    }
    m_type = type;
    emit typeChanged();
}

void Phrase::setType(const QString &typeString)
{
    if (typeString == "word") {
        setType(Word);
        return;
    }
    if (typeString == "expression") {
        setType(Expression);
        return;
    }
    if (typeString == "sentence") {
        setType(Sentence);
        return;
    }
    if (typeString == "paragraph") {
        setType(Paragraph);
        return;
    }
    kWarning() << "Cannot set type from unknown identifier, aborting";
    return;
}

Unit * Phrase::unit() const
{
    return m_unit;
}

void Phrase::setUnit(Unit *unit)
{
    if (unit == m_unit) {
        return;
    }
    m_unit = unit;
    emit unitChanged();
}

KUrl Phrase::sound() const
{
    return KUrl(); //m_sound->currentSource().url();
}

void Phrase::setSound(const KUrl &soundFile)
{
    if (!soundFile.isValid() || soundFile.isEmpty()) {
        kWarning() << "Not setting empty sound file path.";
        return;
    }
    m_soundFile = soundFile;
    emit soundChanged();
}

void Phrase::playbackSound()
{
    kDebug() << "Playing authentic sound";
    m_audioOutput->setMedia(m_soundFile);
    m_audioOutput->setVolume(50); //TODO use global config
    m_audioOutput->play();
    m_currentPlayback = Sound;
}

void Phrase::playbackUserSound()
{
    kDebug() << this << "Playback sound in file "<< m_userSoundFile.fileName();
    m_audioOutput->setMedia(KUrl::fromLocalFile(m_userSoundFile.fileName()));
    m_audioOutput->setVolume(50); //TODO use global config
    m_audioOutput->play();
    m_currentPlayback = UserSound;
}

Phrase::PlaybackState Phrase::playbackSoundState() const
{
    if (m_currentPlayback != Sound) {
        return Phrase::StoppedState;
    }
    switch (m_audioOutput->state())
    {
    case QMediaPlayer::PlayingState:
        return Phrase::PlayingState;
    case QMediaPlayer::PausedState:
        return Phrase::PausedState;
    default:
        return Phrase::StoppedState;
    }
}

Phrase::PlaybackState Phrase::playbackUserSoundState() const
{
    if (m_currentPlayback != UserSound) {
        return Phrase::StoppedState;
    }
    switch (m_audioOutput->state())
    {
    case QMediaPlayer::PlayingState:
        return Phrase::PlayingState;
    case QMediaPlayer::PausedState:
        return Phrase::PausedState;
    default:
        return Phrase::StoppedState;
    }
}

void Phrase::stopSound()
{
    m_audioOutput->stop();
}

bool Phrase::isSound() const
{
    return !m_soundFile.fileName().isEmpty();
}

void Phrase::stopPlaybackUserSound()
{
    m_audioOutput->stop();
}

void Phrase::startRecordUserSound()
{
    kDebug() << "Start recording to file " << m_userSoundFile.fileName();
    CaptureDeviceController::self().startCapture(m_userSoundFile.fileName());
}

void Phrase::stopRecordUserSound()
{
    kDebug() << "End recording to file " << m_userSoundFile.fileName();
    CaptureDeviceController::self().stopCapture();
    emit userSoundChanged();
}

bool Phrase::isUserSound() const
{
    return !m_userSoundFile.fileName().isEmpty();
}

QList<Phoneme *> Phrase::phonemes() const
{
    return m_phonemes;
}

bool Phrase::hasPhoneme(Phoneme* phoneme)
{
    return m_phonemes.contains(phoneme);
}

void Phrase::addPhoneme(Phoneme *phoneme)
{
    if (!m_phonemes.contains(phoneme)) {
        m_phonemes.append(phoneme);
        emit phonemesChanged();
        //FIXME tell Unit to also send corresponding signal!
    }
}

void Phrase::removePhoneme(Phoneme *phoneme)
{
    if (m_phonemes.removeOne(phoneme)) {
        emit phonemesChanged();
        //FIXME tell Unit to also send corresponding signal!
    }
}
