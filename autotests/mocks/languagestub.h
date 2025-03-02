/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef LANGUAGESTUB_H
#define LANGUAGESTUB_H

#include "core/phonemegroup.h"
#include "src/core/ilanguage.h"
#include <QVector>

class Phoneme;
class PhonemeGroup;

class LanguageStub : public ILanguage
{
public:
    explicit LanguageStub(QString id)
        : m_id(id)
    {
    }
    ~LanguageStub() override;

    QString id() const override
    {
        return m_id;
    }
    QString title() const override
    {
        return m_title;
    }
    void setTitle(QString title)
    {
        m_title = title;
        Q_EMIT titleChanged();
    }
    QString i18nTitle() const override
    {
        return QStringLiteral("i18n title");
    }
    QVector<std::shared_ptr<Phoneme>> phonemes() const override
    {
        QVector<std::shared_ptr<Phoneme>> phonemes;
        for (auto group : m_phonemeGroups) {
            phonemes.append(group->phonemes());
        }
        return phonemes;
    }
    QVector<std::shared_ptr<PhonemeGroup>> phonemeGroups() const override
    {
        return m_phonemeGroups;
    }

    std::shared_ptr<PhonemeGroup> addPhonemeGroup(const QString &identifier, const QString &title)
    {
        std::shared_ptr<PhonemeGroup> phonemeGroup(new PhonemeGroup);
        phonemeGroup->setId(identifier);
        phonemeGroup->setTitle(title);
        m_phonemeGroups.append(phonemeGroup);
        return phonemeGroup;
    }

private:
    QString m_id {QStringLiteral("UNKNOWN_ID")};
    QString m_title {QStringLiteral("title")};
    QVector<std::shared_ptr<PhonemeGroup>> m_phonemeGroups;
};

#endif
