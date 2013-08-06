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

#ifndef PHRASE_H
#define PHRASE_H

#include "artikulatecore_export.h"

#include <QObject>
#include <KUrl>
#include <QList>
#include <KTemporaryFile>

namespace Phonon
{
class MediaObject;
}

class QAudioCaptureSource;
class QMediaPlayer;
class QString;
class Phoneme;
class Unit;
class KUrl;

class ARTIKULATELIB_EXPORT Phrase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString i18nText READ i18nText WRITE seti18nText NOTIFY i18nTextChanged)
    Q_PROPERTY(Phrase::Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(Phrase::EditState editState READ editState WRITE setEditState NOTIFY editStateChanged)
    Q_PROPERTY(Unit *unit READ unit NOTIFY unitChanged)
    Q_PROPERTY(KUrl sound READ sound WRITE setSound NOTIFY soundChanged)
    Q_PROPERTY(bool isSound READ isSound NOTIFY soundChanged)
    Q_PROPERTY(bool isUserSound READ isUserSound NOTIFY userSoundChanged)
    Q_PROPERTY(PlaybackState playbackSoundState READ playbackSoundState NOTIFY playbackSoundStateChanged)
    //TODO this is working for now, but implementation must be revisited
    Q_PROPERTY(PlaybackState playbackNativeSoundBufferState READ playbackSoundState NOTIFY playbackSoundStateChanged)
    Q_PROPERTY(PlaybackState playbackUserSoundState READ playbackUserSoundState NOTIFY playbackUserSoundStateChanged)
    Q_PROPERTY(RecordingState recordingState READ recordingState NOTIFY recordingStateChanged)

public:
    Q_ENUMS(EditState)
    Q_ENUMS(Type)
    Q_ENUMS(PlaybackState)
    Q_ENUMS(RecordingState)
    enum PlaybackState {
        StoppedState,
        PlayingState,
        PausedState
    };
    enum RecordingState {
        CurrentlyRecordingState,
        NotRecordingState,
    };
    enum EditState {
        Unknown,
        Translated,
        Completed
    };
    enum Type {
        Word,
        Expression,
        Sentence,
        Paragraph,
        AllTypes
    };
    enum CurrentPlayback {
        None,
        Sound,
        UserSound
    };

    explicit Phrase(QObject *parent = 0);
    ~Phrase();

    QString id() const;
    void setId(const QString &id);
    QString foreignId() const;
    void setForeignId(const QString &id);
    QString text() const;
    void setText(const QString &text);
    QString i18nText() const;
    void seti18nText(const QString &text);
    Unit * unit() const;
    void setUnit(Unit *unit);
    Phrase::Type type() const;
    QString typeString() const;
    void setType(Phrase::Type type);
    void setType(const QString &typeString);
    Phrase::EditState editState() const;
    QString editStateString() const;
    void setEditState(Phrase::EditState state);
    void setEditState(const QString &stateString);
    KUrl sound() const;
    void setSound(const KUrl &soundFile);
    QList<Phoneme *> phonemes() const;

    Q_INVOKABLE bool hasPhoneme(Phoneme *phoneme);
    Q_INVOKABLE void addPhoneme(Phoneme *phoneme);
    Q_INVOKABLE void removePhoneme(Phoneme *phoneme);
    Q_INVOKABLE void playbackSound();
    Q_INVOKABLE void playbackNativeSoundBuffer();
    Q_INVOKABLE void stopSound();
    Q_INVOKABLE void playbackUserSound();
    Q_INVOKABLE void stopPlaybackUserSound();
    /**
     * Start recording interface to record sound to a temporary file.
     * This temporary file will not be saved unless applyRecordedNativeSound()
     * is called.
     */
    Q_INVOKABLE void startRecordNativeSound();
    /**
     * Stop recording of sound. It is save even if currently no sound is recorded.
     */
    Q_INVOKABLE void stopRecordNativeSound();
    /**
     * Overwrite native sound resource file by buffered file.
     */
    Q_INVOKABLE void applyRecordedNativeSound();
    Q_INVOKABLE void startRecordUserSound();
    Q_INVOKABLE void stopRecordUserSound();
    PlaybackState playbackSoundState() const;
    PlaybackState playbackUserSoundState() const;
    RecordingState recordingState() const;

    /**
     * Return true if a sound file exists, otherwise false.
     */
    bool isSound() const;

    /**
     * Return true if a user recorded sound exists, otherwise false.
     */
    bool isUserSound() const;

signals:
    void idChanged();
    void unitChanged();
    void textChanged();
    void i18nTextChanged();
    void typeChanged();
    void editStateChanged();
    void soundChanged();
    void userSoundChanged();
    void phonemesChanged();
    void playbackSoundStateChanged();
    void playbackUserSoundStateChanged();
    void recordingStateChanged();
    void modified();

private slots:
    void updatePlaybackState();

private:
    Q_DISABLE_COPY(Phrase)
    QString m_id;
    QString m_foreignId;
    QString m_text;
    QString m_i18nText;
    Type m_type;
    EditState m_editState;
    RecordingState m_recordingState;
    Unit *m_unit;

    QList<Phoneme *> m_phonemes;
    KUrl m_nativeSoundFile;
    KTemporaryFile m_nativeSoundBuffer;
    KTemporaryFile m_userSoundFile;
    PlaybackState m_nativeSoundPlaybackState;
    PlaybackState m_userSoundPlaybackState;
};
Q_DECLARE_METATYPE(Phrase::PlaybackState)

#endif // PHRASE_H
