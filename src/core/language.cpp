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
#include <QQmlEngine>
#include <QXmlSchema>
#include <QDomDocument>
#include "resources/courseparser.h"

std::shared_ptr<Language> Language::create(QUrl file)
{
    QXmlSchema schema = CourseParser::loadXmlSchema(QStringLiteral("language"));
    if (!schema.isValid()) {
        return nullptr;
    }

    QDomDocument document = CourseParser::loadDomDocument(file, schema);
    if (document.isNull()) {
        qCWarning(ARTIKULATE_LOG) << "Could not parse document " << file.toLocalFile() << ", aborting.";
        return nullptr;
    }

    QDomElement root(document.documentElement());
    auto language = std::shared_ptr<Language>(new Language());
    language->setFile(file);
    language->setId(root.firstChildElement(QStringLiteral("id")).text());
    language->setTitle(root.firstChildElement(QStringLiteral("title")).text());
    language->seti18nTitle(root.firstChildElement(QStringLiteral("i18nTitle")).text());
    // create phoneme groups
    for (QDomElement groupNode = root.firstChildElement(QStringLiteral("phonemeGroups")).firstChildElement();
         !groupNode.isNull();
         groupNode = groupNode.nextSiblingElement())
    {
        auto group = language->addPhonemeGroup(
            groupNode.firstChildElement(QStringLiteral("id")).text(),
            groupNode.firstChildElement(QStringLiteral("title")).text());
        group->setDescription(groupNode.attribute(QStringLiteral("description")));
        // register phonemes
        for (QDomElement phonemeNode = groupNode.firstChildElement(QStringLiteral("phonemes")).firstChildElement();
            !phonemeNode.isNull();
            phonemeNode = phonemeNode.nextSiblingElement())
        {
            group->addPhoneme(phonemeNode.firstChildElement(QStringLiteral("id")).text(), phonemeNode.firstChildElement(QStringLiteral("title")).text());
        }
    }
    return language;
}

Language::Language()
    : QObject()
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}

Language::~Language() = default;

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

QVector<std::shared_ptr<Phoneme>> Language::phonemes() const
{
    QVector<std::shared_ptr<Phoneme>> list;
    for (auto group : m_phonemeGroups) {
        list << group->phonemes();
    }
    return list;
}

QVector<std::shared_ptr<PhonemeGroup>> Language::phonemeGroups() const
{
    return m_phonemeGroups;
}

std::shared_ptr<PhonemeGroup> Language::addPhonemeGroup(const QString &identifier, const QString &title)
{
    for (auto group : m_phonemeGroups) {
        if (QString::compare(group->id(), identifier) == 0) {
            qCWarning(ARTIKULATE_LOG) << "Pronunciation Group identifier already registered, aborting";
            return std::shared_ptr<PhonemeGroup>();
        }
    }

    std::shared_ptr<PhonemeGroup> phonemeGroup(new PhonemeGroup);
    phonemeGroup->setId(identifier);
    phonemeGroup->setTitle(title);
    m_phonemeGroups.append(phonemeGroup);

    connect(phonemeGroup.get(), &PhonemeGroup::phonemeAdded, this, &Language::phonemesChanged);
    emit phonemeGroupsChanged();

    return phonemeGroup;
}
