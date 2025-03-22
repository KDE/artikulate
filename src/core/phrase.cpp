/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "phrase.h"
#include "artikulate_debug.h"
#include "icourse.h"
#include "unit.h"
#include <QQmlEngine>

Phrase::Phrase()
    : IEditablePhrase()
    , m_type(IPhrase::Type::AllTypes)
    , m_editState(IEditablePhrase::EditState::Unknown)
    , m_trainingProgress(0)
    , m_skipCounter(0)
    , m_excludedFromUnit(false)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

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

std::shared_ptr<Phrase> Phrase::create()
{
    std::shared_ptr<Phrase> phrase(new Phrase);
    phrase->setSelf(phrase);
    return phrase;
}

void Phrase::setSelf(std::shared_ptr<IPhrase> self)
{
    m_self = self;
}

std::shared_ptr<IPhrase> Phrase::self() const
{
    return m_self.lock();
}

QString Phrase::id() const
{
    return m_id;
}

void Phrase::setId(QString id)
{
    if (id != m_id) {
        m_id = std::move(id);
        Q_EMIT idChanged();
    }
}

QString Phrase::foreignId() const
{
    return m_foreignId;
}

void Phrase::setForeignId(QString id)
{
    m_foreignId = std::move(id);
}

QString Phrase::text() const
{
    return m_text;
}

void Phrase::setText(QString text)
{
    if (QString::compare(text, m_text) != 0) {
        m_text = text.trimmed();
        Q_EMIT textChanged();
    }
}

QString Phrase::i18nText() const
{
    return m_i18nText;
}

void Phrase::seti18nText(QString text)
{
    if (QString::compare(text, m_i18nText) != 0) {
        // copy unmodified original text string
        m_i18nText = std::move(text);
        Q_EMIT i18nTextChanged();
    }
}

Phrase::Type Phrase::type() const
{
    return m_type;
}

QString Phrase::typeString() const
{
    switch (m_type) {
    case IPhrase::Type::Word:
        return QStringLiteral("word");
    case IPhrase::Type::Expression:
        return QStringLiteral("expression");
    case IPhrase::Type::Sentence:
        return QStringLiteral("sentence");
    case IPhrase::Type::Paragraph:
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
    Q_EMIT typeChanged();
}

void Phrase::setType(const QString &typeString)
{
    if (typeString == QLatin1String("word")) {
        setType(IPhrase::Type::Word);
        return;
    }
    if (typeString == QLatin1String("expression")) {
        setType(IPhrase::Type::Expression);
        return;
    }
    if (typeString == QLatin1String("sentence")) {
        setType(IPhrase::Type::Sentence);
        return;
    }
    if (typeString == QLatin1String("paragraph")) {
        setType(IPhrase::Type::Paragraph);
        return;
    }
    qCWarning(ARTIKULATE_CORE()) << "Cannot set type from unknown identifier, aborting";
    return;
}

Phrase::EditState Phrase::editState() const
{
    return m_editState;
}

QString Phrase::editStateString() const
{
    switch (m_editState) {
    case IEditablePhrase::EditState::Unknown:
        return QStringLiteral("unknown");
    case IEditablePhrase::EditState::Translated:
        return QStringLiteral("translated");
    case IEditablePhrase::EditState::Completed:
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
    Q_EMIT editStateChanged();
}

void Phrase::setEditState(const QString &stateString)
{
    if (stateString.isEmpty()) {
        return;
    }
    if (stateString == QLatin1String("unknown")) {
        setEditState(IEditablePhrase::EditState::Unknown);
        return;
    }
    if (stateString == QLatin1String("translated")) {
        setEditState(IEditablePhrase::EditState::Translated);
        return;
    }
    if (stateString == QLatin1String("completed")) {
        setEditState(IEditablePhrase::EditState::Completed);
        return;
    }
    qCWarning(ARTIKULATE_LOG) << "Cannot set edit state from unknown identifier " << stateString << ", aborting";
    return;
}

std::shared_ptr<IUnit> Phrase::unit() const
{
    return m_unit.lock();
}

void Phrase::setUnit(std::shared_ptr<IUnit> unit)
{
    Q_ASSERT(unit);
    if (unit == m_unit.lock()) {
        return;
    }
    m_unit = unit;
    Q_EMIT unitChanged();
}

std::shared_ptr<IPhrase> Phrase::next() const
{
    if (m_unit.lock() == nullptr) {
        qCritical() << "Phrase has no unit parent, abort previous computation";
        return std::shared_ptr<IPhrase>();
    }
    qsizetype phraseIndex{0};
    auto unitPhrases = m_unit.lock()->phrases();
    for (qsizetype i = 0; i < unitPhrases.size(); ++i) {
        if (unitPhrases.at(i).get() == this) {
            phraseIndex = i;
            break;
        }
    }
    if (phraseIndex < unitPhrases.size() - 1) {
        return unitPhrases.at(phraseIndex + 1);
    } else {
        qsizetype unitIndex{0};
        auto unit = m_unit.lock();
        if (!unit || !unit->course()) {
            qCritical() << "could not obtain unit or parent course, aborting" << unit->course().get();
            return std::shared_ptr<IPhrase>();
        }
        auto units = unit->course()->units();
        for (qsizetype i = 0; i < units.size(); ++i) {
            if (units.at(i).get() == unit.get()) {
                unitIndex = i;
                break;
            }
        }
        if (unitIndex < units.size() - 1 && !units.at(unitIndex + 1)->phrases().isEmpty()) {
            return units.at(unitIndex + 1)->phrases().first();
        }
    }
    return std::shared_ptr<IPhrase>();
}

std::shared_ptr<IPhrase> Phrase::previous() const
{
    if (m_unit.lock() == nullptr) {
        qCritical() << "Phrase has no unit parent, abort previous computation";
        return std::shared_ptr<IPhrase>();
    }
    qsizetype phraseIndex{0};
    auto unitPhrases = m_unit.lock()->phrases();
    for (qsizetype i = 0; i < unitPhrases.size(); ++i) {
        if (unitPhrases.at(i).get() == this) {
            phraseIndex = i;
            break;
        }
    }
    if (phraseIndex > 0) {
        return unitPhrases.at(phraseIndex - 1);
    } else {
        qsizetype unitIndex{0};
        auto unit = m_unit.lock();
        if (!unit || !unit->course()) {
            qCritical() << "could not obtain unit or parent course, aborting";
            return std::shared_ptr<IPhrase>();
        }
        auto units = unit->course()->units();
        for (qsizetype i = 0; i < units.size(); ++i) {
            if (units.at(i).get() == unit.get()) {
                unitIndex = i;
                break;
            }
        }
        if (unitIndex > 0 && !units.at(unitIndex - 1)->phrases().isEmpty()) {
            return units.at(unitIndex - 1)->phrases().last();
        }
    }
    return std::shared_ptr<IPhrase>();
}

QUrl Phrase::sound() const
{
    return m_nativeSoundFile;
}

void Phrase::setSound(QUrl soundFile)
{
    if (!soundFile.isValid() || soundFile.isEmpty()) {
        qCWarning(ARTIKULATE_LOG) << "Not setting empty sound file path.";
        return;
    }
    m_nativeSoundFile = std::move(soundFile);
    Q_EMIT soundChanged();
}

QString Phrase::soundFileOutputPath() const
{
    if (m_nativeSoundFile.isEmpty()) {
        QString outputDir = m_unit.lock()->course()->file().path() + QLatin1Char('/');
        // TODO take care that this is proper ASCII
        return outputDir + id() + QStringLiteral(".ogg");
    } else {
        return m_nativeSoundFile.toLocalFile();
    }
}

void Phrase::markSoundRecorded()
{
    if (soundFileOutputPath() != m_nativeSoundFile.toLocalFile()) {
        m_nativeSoundFile = QUrl::fromLocalFile(soundFileOutputPath());
        Q_EMIT soundChanged();
        Q_EMIT modified();
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
    Q_EMIT excludedChanged();
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
    Q_EMIT progressChanged();
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
            Q_EMIT progressChanged();
        }
        return;
    }
    if (progress == Progress::Skip) {
        ++m_skipCounter;
        if (m_skipCounter > 2 && m_trainingProgress > 0) {
            --m_trainingProgress;
            Q_EMIT progressChanged();
        }
        return;
    }
}

QVector<Phoneme *> Phrase::phonemes() const
{
    return m_phonemes;
}

bool Phrase::hasPhoneme(Phoneme *phoneme)
{
    return m_phonemes.contains(phoneme);
}

void Phrase::addPhoneme(Phoneme *phoneme)
{
    if (!m_phonemes.contains(phoneme)) {
        m_phonemes.append(phoneme);
        Q_EMIT phonemesChanged();
        // FIXME tell Unit to also send corresponding signal!
    }
}

void Phrase::removePhoneme(Phoneme *phoneme)
{
    if (m_phonemes.removeOne(phoneme)) {
        Q_EMIT phonemesChanged();
        // FIXME tell Unit to also send corresponding signal!
    }
}
