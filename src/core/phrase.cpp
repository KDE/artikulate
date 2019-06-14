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
#include "icourse.h"
#include "settings.h"

#include "artikulate_debug.h"
#include <QTemporaryFile>

Phrase::Phrase(QObject *parent)
    : QObject(parent)
    , m_type(Phrase::AllTypes)
    , m_editState(Unknown)
    , m_unit(nullptr)
    , m_trainingProgress(0)
    , m_skipCounter(0)
    , m_excludedFromUnit(false)
{
    connect(this, &Phrase::idChanged, this, &Phrase::modified);
    connect(this, &Phrase::typeChanged, this, &Phrase::modified);
    connect(this, &Phrase::textChanged, this, &Phrase::modified);
    connect(this, &Phrase::soundChanged, this, &Phrase::modified);
    connect(this, &Phrase::editStateChanged, this, &Phrase::modified);
    connect(this, &Phrase::i18nTextChanged, this, &Phrase::modified);
    connect(this, &Phrase::phonemesChanged, this, &Phrase::modified);
    connect(this, &Phrase::excludedChanged, this, &Phrase::modified);
}

Phrase::~Phrase() = default;

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
        return QStringLiteral("word");
    case Expression:
        return QStringLiteral("expression");
    case Sentence:
        return QStringLiteral("sentence");
    case Paragraph:
        return QStringLiteral("paragraph");
    default:
        return QStringLiteral("ERROR_UNKNOWN_TYPE");
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
    if (typeString == QLatin1String("word")) {
        setType(Word);
        return;
    }
    if (typeString == QLatin1String("expression")) {
        setType(Expression);
        return;
    }
    if (typeString == QLatin1String("sentence")) {
        setType(Sentence);
        return;
    }
    if (typeString == QLatin1String("paragraph")) {
        setType(Paragraph);
        return;
    }
    qCWarning(ARTIKULATE_LOG) << "Cannot set type from unknown identifier, aborting";
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
        return QStringLiteral("unknown");
    case Translated:
        return QStringLiteral("translated");
    case Completed:
        return QStringLiteral("completed");
    }
    Q_UNREACHABLE();
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
    if (stateString == QLatin1String("unknown")) {
        setEditState(Unknown);
        return;
    }
    if (stateString == QLatin1String("translated")) {
        setEditState(Translated);
        return;
    }
    if (stateString == QLatin1String("completed")) {
        setEditState(Completed);
        return;
    }
    qCWarning(ARTIKULATE_LOG) << "Cannot set edit state from unknown identifier " << stateString << ", aborting";
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

QUrl Phrase::sound() const
{
    return m_nativeSoundFile;
}

void Phrase::setSound(const QUrl &soundFile)
{
    if (!soundFile.isValid() || soundFile.isEmpty()) {
        qCWarning(ARTIKULATE_LOG) << "Not setting empty sound file path.";
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

int Phrase::progress() const
{
    return static_cast<int>(m_trainingProgress);
}

void Phrase::setProgress(int value)
{
    Q_ASSERT(value >= 0);
    if (value < 0) {
        value = 0;
    }
    if (m_trainingProgress == static_cast<uint>(value)) {
        return;
    }
    m_trainingProgress = static_cast<uint>(value);
    emit progressChanged();
}
void Phrase::updateProgress(Phrase::Progress progress)
{
    // logic of progress computation:
    // a) if skipped 3 times in a row, decrease progress
    // b) if done and skipped less than two times in a row, increase progress
    if (progress == Progress::Done) {
        m_skipCounter = 0;
        if (m_trainingProgress < 3) {
            ++m_trainingProgress;
            emit progressChanged();
        }
        return;
    }
    if (progress == Progress::Skip) {
        ++m_skipCounter;
        if (m_skipCounter > 2 && m_trainingProgress > 0) {
            --m_trainingProgress;
            emit progressChanged();
        }
        return;
    }
}

QVector<Phoneme *> Phrase::phonemes() const
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
