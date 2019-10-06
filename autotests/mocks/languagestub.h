/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef LANGUAGESTUB_H
#define LANGUAGESTUB_H

#include "core/phonemegroup.h"
#include "src/core/ilanguage.h"
#include <QObject>
#include <QVector>

class Phoneme;
class PhonemeGroup;

class LanguageStub : public ILanguage
{
public:
    LanguageStub(QString id)
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
        emit titleChanged();
    }
    QString i18nTitle() const override
    {
        return "i18n title";
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
    QString m_id {"UNKNOWN_ID"};
    QString m_title {"title"};
    QVector<std::shared_ptr<PhonemeGroup>> m_phonemeGroups;
};

#endif
