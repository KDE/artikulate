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
#include "libsound/src/capturedevicecontroller.h"
#include "libsound/src/outputdevicecontroller.h"
#include "unit.h"
#include "course.h"
#include "settings.h"

#include <QDebug>
#include <KSaveFile>
#include <KTemporaryFile>
#include <sys/stat.h>

Phrase::Phrase(QObject *parent)
    : QObject(parent)
    , m_type(Phrase::AllTypes)
    , m_editState(Unknown)
    , m_trainingState(Untrained)
    , m_unit(0)
    , m_excludedFromUnit(false)
{

}

Phrase::~Phrase()
{

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
    qWarning() << "Cannot set type from unknown identifier, aborting";
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
    qWarning() << "Cannot set edit state from unknown identifier " << stateString << ", aborting";
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

QUrl Phrase::sound() const
{
    return m_nativeSoundFile;
}

void Phrase::setSound(const QUrl &soundFile)
{
    if (!soundFile.isValid() || soundFile.isEmpty()) {
        qWarning() << "Not setting empty sound file path.";
        return;
    }
    m_nativeSoundFile = soundFile;
    emit soundChanged();
}

QString Phrase::soundFileUrl() const
{
    return m_nativeSoundFile.toLocalFile();
}

QString Phrase::soundFileOutputPath() const
{
    if (m_nativeSoundFile.isEmpty()) {
        QString outputDir = m_unit->course()->file().path() + '/';
        //TODO take care that this is proper ASCII
        return outputDir + id() + ".ogg";
    } else {
        return soundFileUrl();
    }
}

void Phrase::setSoundFileUrl()
{
    if (soundFileOutputPath() != m_nativeSoundFile.toLocalFile()) {
        m_nativeSoundFile = QUrl::fromLocalFile(soundFileOutputPath());
        emit soundChanged();
        emit modified();
    }
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
