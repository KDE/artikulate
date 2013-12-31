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

#include "phrase.h"
#include "capturedevicecontroller.h"
#include "outputdevicecontroller.h"
#include "unit.h"
#include "course.h"
#include "settings.h"

#include <phonon/AudioOutput>
#include <phonon/MediaObject>

#include <KDebug>
#include <KSaveFile>
#include <KTemporaryFile>
#include <sys/stat.h>

Phrase::Phrase(QObject *parent)
    : QObject(parent)
    , m_editState(Unknown)
    , m_trainingState(Untrained)
    , m_excludedFromUnit(false)
    , m_nativeSoundPlaybackState(StoppedState)
    , m_userSoundPlaybackState(StoppedState)
{
    // register recording file
    m_userSoundFile.setSuffix(".ogg");
}

Phrase::~Phrase()
{
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
        m_text = text.trimmed();
        emit textChanged();
    }
}

QString Phrase::i18nText() const
{
    return m_i18nText;
}

void Phrase::seti18nText(const QString &text)
{
    if (QString::compare(text, m_i18nText) != 0) {
        // copy unmodified original text string
        m_i18nText = text;
        emit i18nTextChanged();
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

Phrase::EditState Phrase::editState() const
{
    return m_editState;
}

QString Phrase::editStateString() const
{
    switch(m_editState) {
    case Unknown:
        return "unknown";
    case Translated:
        return "translated";
    case Completed:
        return "completed";
    default:
        return "ERROR_UNKNOWN_EDIT_STATE";
    }
}

void Phrase::setEditState(Phrase::EditState state)
{
    if (m_editState == state) {
        return;
    }
    m_editState = state;
    emit editStateChanged();
}

void Phrase::setEditState(const QString &stateString)
{
    if (stateString.isEmpty()) {
        return;
    }
    if (stateString == "unknown") {
        setEditState(Unknown);
        return;
    }
    if (stateString == "translated") {
        setEditState(Translated);
        return;
    }
    if (stateString == "completed") {
        setEditState(Completed);
        return;
    }
    kWarning() << "Cannot set edit state from unknown identifier " << stateString << ", aborting";
    return;
}

Phrase::TrainingState Phrase::trainingState() const
{
    return m_trainingState;
}

void Phrase::setTrainingState(Phrase::TrainingState state)
{
    m_trainingState = state;
    emit trainingStateChanged();
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
    return m_nativeSoundFile;
}

void Phrase::setSound(const KUrl &soundFile)
{
    if (!soundFile.isValid() || soundFile.isEmpty()) {
        kWarning() << "Not setting empty sound file path.";
        return;
    }
    m_nativeSoundFile = soundFile;
    emit soundChanged();
}

QString Phrase::soundFileUrl() const
{
    return m_nativeSoundFile.toLocalFile();
}

void Phrase::playbackSound()
{
    kDebug() << "Playing authentic sound";
    OutputDeviceController::self().play(m_nativeSoundFile);
    m_nativeSoundPlaybackState = PlayingState;
    connect(&OutputDeviceController::self(), SIGNAL(stopped()), this, SLOT(updatePlaybackState()));
    emit playbackSoundStateChanged();
}

void Phrase::playbackNativeSoundBuffer()
{
    kDebug() << "Playing sound buffer";
    OutputDeviceController::self().play(KUrl::fromLocalFile(m_nativeSoundBuffer.fileName()));
}

void Phrase::playbackUserSound()
{
    kDebug() << this << "Playback sound in file "<< m_userSoundFile.fileName();
    OutputDeviceController::self().play(KUrl::fromLocalFile(m_userSoundFile.fileName()));
    m_userSoundPlaybackState = PlayingState;
    connect(&OutputDeviceController::self(), SIGNAL(stopped()), this, SLOT(updatePlaybackState()));
    emit playbackUserSoundStateChanged();
}

Phrase::PlaybackState Phrase::playbackSoundState() const
{
    return m_nativeSoundPlaybackState;
}

Phrase::PlaybackState Phrase::playbackUserSoundState() const
{
    return m_userSoundPlaybackState;
}

Phrase::RecordingState Phrase::recordingState() const
{
    return m_recordingState;
}

void Phrase::stopSound()
{
    OutputDeviceController::self().stop();
    OutputDeviceController::self().disconnect();
    emit playbackSoundStateChanged();
}

bool Phrase::isSound() const
{
    return !m_nativeSoundFile.fileName().isEmpty();
}

void Phrase::stopPlaybackUserSound()
{
    OutputDeviceController::self().stop();
    OutputDeviceController::self().disconnect();
    emit playbackUserSoundStateChanged();
}

void Phrase::updatePlaybackState()
{
    if (!OutputDeviceController::self().state() == Phonon::StoppedState) {
        return;
    }
    if (playbackSoundState() == PlayingState) {
        m_nativeSoundPlaybackState = StoppedState;
        emit playbackSoundStateChanged();
    }
    if (playbackUserSoundState() == PlayingState) {
        m_userSoundPlaybackState = StoppedState;
        emit playbackUserSoundStateChanged();
    }
    OutputDeviceController::self().disconnect();
}

void Phrase::startRecordNativeSound()
{
    m_nativeSoundBuffer.open();
    m_recordingState = CurrentlyRecordingState;
    emit recordingStateChanged();

    kDebug() << "Start recording to temporary file " << m_nativeSoundBuffer.fileName();
    CaptureDeviceController::self().startCapture(m_nativeSoundBuffer.fileName());
}

void Phrase::stopRecordNativeSound()
{
    kDebug() << "End recording to temporary file " << m_nativeSoundBuffer.fileName();
    CaptureDeviceController::self().stopCapture();
    m_recordingState = NotRecordingState;
    emit recordingStateChanged();
    emit soundChanged();
}

void Phrase::applyRecordedNativeSound()
{
    if (m_nativeSoundFile.isEmpty()) {
        QString outputDir = m_unit->course()->file().directory(KUrl::AppendTrailingSlash);
        kDebug() << "Target directory: " << outputDir;

        //TODO take care that this is proper ASCII
        m_nativeSoundFile = KUrl::fromLocalFile(outputDir + id() + ".ogg");
        kDebug() << "preparing write to file " << m_nativeSoundFile.toLocalFile();
    }

    if (m_nativeSoundBuffer.isOpen()) {
        kDebug() << "Saving buffered data.";
        QFile targetFile;
        targetFile.setFileName(m_nativeSoundFile.toLocalFile());
        if (!targetFile.exists() || targetFile.remove()) {
            m_nativeSoundBuffer.copy(m_nativeSoundFile.toLocalFile());
            m_nativeSoundBuffer.close();
        } else {
            kError() << "Could not save buffered sound data to file, data loss!";
        }
    } else {
        kError() << "No buffer present.";
    }
    emit soundChanged();
}

void Phrase::startRecordUserSound()
{
    if(!m_userSoundFile.isOpen()) {
        m_userSoundFile.open();
    }
    kDebug() << "Start recording user sound to file " << m_userSoundFile.fileName();
    CaptureDeviceController::self().startCapture(m_userSoundFile.fileName());
    m_recordingState = CurrentlyRecordingState;
    emit recordingStateChanged();
    m_userSoundFile.close();
}

void Phrase::stopRecordUserSound()
{
    kDebug() << "End recording user sound to file " << m_userSoundFile.fileName();
    CaptureDeviceController::self().stopCapture();
    m_recordingState = NotRecordingState;
    emit recordingStateChanged();
    emit userSoundChanged();
}

bool Phrase::isUserSound() const
{
    return !m_userSoundFile.fileName().isEmpty();
}

bool Phrase::isExcluded() const
{
    return m_excludedFromUnit;
}

void Phrase::setExcluded(bool excluded)
{
    if (excluded == m_excludedFromUnit) {
        return;
    }
    m_excludedFromUnit = excluded;
    emit excludedChanged();
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
