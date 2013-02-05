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

#ifndef PHRASE_H
#define PHRASE_H

#include "artikulatecore_export.h"

#include <QObject>
#include <KUrl>
#include <QList>
#include <QFile>
#include <phonon/mediaobject.h>

class QMediaPlayer;
class QAudioInput;
class QString;
class QMediaRecorder;
class Tag;
class KUrl;

class ARTIKULATELIB_EXPORT Phrase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(KUrl sound READ sound WRITE setSound NOTIFY soundChanged)
    Q_PROPERTY(bool isUserSound READ isUserSound NOTIFY userSoundChanged)
    Q_PROPERTY(PlaybackState playbackSoundState READ playbackSoundState NOTIFY playbackSoundStateChanged)
    Q_PROPERTY(PlaybackState playbackUserSoundState READ playbackUserSoundState NOTIFY playbackUserSoundStateChanged)

public:
    Q_ENUMS(Type)
    Q_ENUMS(PlaybackState)
    enum PlaybackState {
        StoppedState,
        PlayingState,
        PausedState
    };
    enum Type {
        Word,
        Expression,
        Sentence,
        Paragraph
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
    QString text() const;
    void setText(const QString &title);
    Phrase::Type type() const;
    QString typeString() const;
    void setType(Phrase::Type type);
    void setType(const QString &typeString);
    KUrl sound() const;
    void setSound(const KUrl &soundFile);
    QList<Tag *> tags() const;
    void addTag(Tag *tag);

    Q_INVOKABLE void playbackSound();
    Q_INVOKABLE void stopSound();
    Q_INVOKABLE void playbackUserSound();
    Q_INVOKABLE void stopPlaybackUserSound();
    Q_INVOKABLE void startRecordUserSound();
    Q_INVOKABLE void stopRecordUserSound();
    PlaybackState playbackSoundState() const;
    PlaybackState playbackUserSoundState() const;

    /**
     * Return true if a user recorded sound exists, otherwise fals.
     */
    bool isUserSound() const;

signals:
    void idChanged();
    void unitChanged();
    void textChanged();
    void soundChanged();
    void userSoundChanged();
    void prononciationTagsChanged();
    void playbackSoundStateChanged();
    void playbackUserSoundStateChanged();

private:
    Q_DISABLE_COPY(Phrase)
    QString m_id;
    QString m_text;
    Type m_type;

    QList<Tag *> m_prononciationTags;
    KUrl m_soundFile;
    KUrl m_userSoundFile;
    CurrentPlayback m_currentPlayback;
    QMediaPlayer *m_audioOutput;
    QMediaRecorder *m_audioInput;
};
Q_DECLARE_METATYPE(Phrase::PlaybackState)

#endif // LESSON_H