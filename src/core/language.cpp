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

#include "language.h"
#include "models/languagemodel.h"
#include "phoneme.h"
#include "phonemegroup.h"
#include "artikulate_debug.h"
#include <KLocalizedString>

Language::Language()
    : QObject()
{
}

Language::~Language()
{
    qDeleteAll(m_phonemeGroups);
}

QString Language::id() const
{
    return m_id;
}

void Language::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        emit idChanged();
    }
}

QString Language::title() const
{
    return m_title;
}

void Language::setTitle(const QString &title)
{
    if (QString::compare(title, m_title) != 0) {
        m_title = title;
        emit titleChanged();
    }
}

QString Language::i18nTitle() const
{
    return i18n(m_i18nTitle.toUtf8());
}

void Language::seti18nTitle(const QString &title)
{
    if (m_i18nTitle == title) {
        return;
    }
    m_i18nTitle = title;
    emit i18nTitleChanged();
}

QUrl Language::file() const
{
    return m_file;
}

void Language::setFile(const QUrl &file)
{
    m_file = file;
}

QList<Phoneme *> Language::phonemes() const
{
    QList<Phoneme *> list;
    foreach (PhonemeGroup *group, m_phonemeGroups) {
        list.append(group->phonemes());
    }
    return list;
}

QList<PhonemeGroup*> Language::phonemeGroups() const
{
    return m_phonemeGroups;
}

PhonemeGroup * Language::addPhonemeGroup(const QString &identifier, const QString &title)
{
    QList<PhonemeGroup *>::ConstIterator iter = m_phonemeGroups.constBegin();
    while (iter != m_phonemeGroups.constEnd()) {
        if (QString::compare((*iter)->id(), identifier) == 0) {
            qCWarning(ARTIKULATE_LOG) << "Pronunciation Group identifier already registered, aborting";
            return nullptr;
        }
        ++iter;
    }

    PhonemeGroup *newGroup = new PhonemeGroup();
    newGroup->setId(identifier);
    newGroup->setTitle(title);
    m_phonemeGroups.append(newGroup);

    connect(newGroup, &PhonemeGroup::phonemeAdded, this, &Language::phonemesChanged);
    connect(newGroup, &PhonemeGroup::phonemeRemoved, this, &Language::phonemesChanged);

    emit phonemeGroupsChanged();

    return newGroup;
}
