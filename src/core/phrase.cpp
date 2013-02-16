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

#include <QAudioInput>
#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QAudioCaptureSource>
#include <QMediaPlayer>
#include <QMediaRecorder>
#include <KDebug>
#include <KTemporaryFile>

Phrase::Phrase(QObject *parent)
    : QObject(parent)
    , m_audioOutput(new QMediaPlayer)
    , m_audioInput(0)
{
    m_currentPlayback = None;
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
    kDebug() << "Playing authentic sound";
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
    // create temporary file for user sound
    if (m_userSoundFile.fileName().isEmpty()) {
        m_userSoundFile.setSuffix(".ogg");
        m_userSoundFile.open();

        kDebug() << "Create user sound file at " << m_userSoundFile.fileName();
    }

    if (!m_audioInput) {
        kDebug() << "Creating audio input device";

        QAudioCaptureSource *audioSource = new QAudioCaptureSource(this);
        m_audioInput = new QMediaRecorder(audioSource, this);

        kDebug() << "AUDIO-SOURCE";
        kDebug() << "available : " << audioSource->isAvailable();
        kDebug() << "inputs : " << audioSource->audioInputs();
        kDebug() << "active input : " << audioSource->activeAudioInput();
        kDebug() << "Available codecs : " << m_audioInput->supportedAudioCodecs();
    }
    if (m_audioInput->state() == QMediaRecorder::RecordingState) {
        kWarning() << "Currently recording, aborting record start";
        return;
    }

    // set output location
    //FIXME for a really strange reason, only the following notation works to get a correct
    // output file; neither QUrl::fromLocalFile, nor the KUrl equivalents are working
    // --> investigate why!
    m_audioInput->setOutputLocation(QUrl(m_userSoundFile.fileName()));

    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/vorbis");
    audioSettings.setSampleRate(0);
    audioSettings.setBitRate(0);
    audioSettings.setQuality(QtMultimediaKit::NormalQuality);
    audioSettings.setEncodingMode(QtMultimediaKit::ConstantQualityEncoding);
    QString container = "ogg";

    m_audioInput->setEncodingSettings(audioSettings, QVideoEncoderSettings(), container);

    kDebug() << "Start recording";
    m_audioInput->record();
}

void Phrase::stopRecordUserSound()
{
    kDebug() << "Stop recording sound";
    m_audioInput->stop();

    delete m_audioInput;
    m_audioInput = 0;
    emit userSoundChanged();
}

bool Phrase::isUserSound() const
{
    return !m_userSoundFile.fileName().isEmpty();
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
