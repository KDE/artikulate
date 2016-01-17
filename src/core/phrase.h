/*
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
#include <QUrl>
#include <QList>
#include <QTemporaryFile>

class QString;
class Phoneme;
class Unit;
class QUrl;

class ARTIKULATECORE_EXPORT Phrase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString i18nText READ i18nText WRITE seti18nText NOTIFY i18nTextChanged)
    Q_PROPERTY(QString soundFileUrl READ soundFileUrl NOTIFY soundChanged)
    Q_PROPERTY(Phrase::Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(Phrase::EditState editState READ editState WRITE setEditState NOTIFY editStateChanged)
    Q_PROPERTY(Unit *unit READ unit NOTIFY unitChanged)
    Q_PROPERTY(bool excluded READ isExcluded NOTIFY excludedChanged)
public:
    Q_ENUMS(EditState)
    Q_ENUMS(TrainingState)
    Q_ENUMS(Type)
    enum EditState {
        Unknown,
        Translated,
        Completed
    };
    enum TrainingState { //TODO not needed anymore with statistics
        Trained,
        Untrained
    };
    enum Type {
        Word,
        Expression,
        Sentence,
        Paragraph,
        AllTypes
    };

    explicit Phrase(QObject *parent = nullptr);
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
    QString soundFileUrl() const;
    Q_INVOKABLE QString soundFileOutputPath() const;
    Q_INVOKABLE void setSoundFileUrl();
    Phrase::EditState editState() const;
    QString editStateString() const;
    void setEditState(Phrase::EditState state);
    void setEditState(const QString &stateString);
    QUrl sound() const;
    void setSound(const QUrl &soundFile);
    QList<Phoneme *> phonemes() const;
    bool isExcluded() const;
    void setExcluded(bool excluded = false);

    Q_INVOKABLE bool hasPhoneme(Phoneme *phoneme);
    Q_INVOKABLE void addPhoneme(Phoneme *phoneme);
    Q_INVOKABLE void removePhoneme(Phoneme *phoneme);

Q_SIGNALS:
    void idChanged();
    void unitChanged();
    void textChanged();
    void i18nTextChanged();
    void typeChanged();
    void editStateChanged();
    void soundChanged();
    void excludedChanged();
    void phonemesChanged();
    void modified();

private:
    Q_DISABLE_COPY(Phrase)
    QString m_id;
    QString m_foreignId;
    QString m_text;
    QString m_i18nText;
    Type m_type;
    EditState m_editState;
    Unit *m_unit;
    bool m_excludedFromUnit;
    QList<Phoneme *> m_phonemes;
    QUrl m_nativeSoundFile;
};

#endif // PHRASE_H
