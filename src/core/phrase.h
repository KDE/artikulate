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
#include "ieditablephrase.h"
#include "iphrase.h"
#include "iunit.h"
#include <QList>
#include <QUrl>
#include <QVector>
#include <memory>

class QString;
class Phoneme;
class IUnit;
class QUrl;

class ARTIKULATECORE_EXPORT Phrase : public IEditablePhrase
{
    Q_OBJECT
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QString i18nText READ i18nText WRITE seti18nText NOTIFY i18nTextChanged)
    Q_PROPERTY(QString soundFileUrl READ soundFileUrl NOTIFY soundChanged)
    Q_PROPERTY(IPhrase::Type type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(Phrase::EditState editState READ editState WRITE setEditState NOTIFY editStateChanged)
    Q_PROPERTY(bool excluded READ isExcluded NOTIFY excludedChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
public:
    enum class Progress { Skip, Done };
    Q_ENUM(Progress)

    static std::shared_ptr<Phrase> create();
    std::shared_ptr<IPhrase> self() const override;

    ~Phrase() override;

    QString id() const override;
    void setId(QString id) override;
    QString foreignId() const override;
    void setForeignId(QString id) override;
    QString text() const override;
    void setText(QString text) override;
    QString i18nText() const override;
    void seti18nText(QString text) override;
    std::shared_ptr<IUnit> unit() const override;
    void setUnit(std::shared_ptr<IUnit> unit) override;
    IPhrase::Type type() const override;
    QString typeString() const override;
    void setType(IPhrase::Type type) override;
    void setType(const QString &typeString);
    QString soundFileUrl() const override;
    Q_INVOKABLE QString soundFileOutputPath() const;
    Q_INVOKABLE void setSoundFileUrl() override;
    IEditablePhrase::EditState editState() const override;
    QString editStateString() const override;
    void setEditState(IEditablePhrase::EditState state) override;
    void setEditState(const QString &stateString) override;
    QUrl sound() const override;
    void setSound(QUrl soundFile) override;
    QVector<Phoneme *> phonemes() const override;
    bool isExcluded() const;
    void setExcluded(bool excluded = false);
    int progress() const;
    void setProgress(int value);
    void updateProgress(Phrase::Progress progress);

    Q_INVOKABLE bool hasPhoneme(Phoneme *phoneme);
    Q_INVOKABLE void addPhoneme(Phoneme *phoneme);
    Q_INVOKABLE void removePhoneme(Phoneme *phoneme);

Q_SIGNALS:
    void progressChanged();
    void excludedChanged();

private:
    Q_DISABLE_COPY(Phrase)
    explicit Phrase();
    void setSelf(std::shared_ptr<IPhrase> self) override;
    std::weak_ptr<IPhrase> m_self;
    QString m_id;
    QString m_foreignId;
    QString m_text;
    QString m_i18nText;
    IPhrase::Type m_type;
    EditState m_editState;
    std::weak_ptr<IUnit> m_unit;
    unsigned m_trainingProgress;
    int m_skipCounter; // count how many skips occurred since last progress update
    bool m_excludedFromUnit;
    QVector<Phoneme *> m_phonemes;
    QUrl m_nativeSoundFile;
};

#endif // PHRASE_H
