/*
 *  SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "language.h"
#include "artikulate_debug.h"
#include "models/languagemodel.h"
#include "phoneme.h"
#include "phonemegroup.h"
#include <KLocalizedString>
#include <QFile>
#include <QIODevice>
#include <QQmlEngine>
#include <QXmlStreamReader>

std::shared_ptr<Language> Language::create(QUrl file)
{
    auto language = std::shared_ptr<Language>(new Language());
    // load basic information from language file, but does not parse everything
    QXmlStreamReader xml;
    QFile handle(file.toLocalFile());
    if (handle.open(QIODevice::ReadOnly)) {
        xml.setDevice(&handle);
        xml.readNextStartElement();
        while (xml.readNext() && !xml.atEnd()) {
            if (xml.name() == "id") {
                language->setId(xml.readElementText());
            }
            if (xml.name() == "title") {
                language->setTitle(xml.readElementText());
            }
            if (xml.name() == "i18nTitle") {
                language->seti18nTitle(xml.readElementText());
            }
            if (xml.name() == "phonemeGroup") {
                std::shared_ptr<PhonemeGroup> group = std::shared_ptr<PhonemeGroup>(new PhonemeGroup);
                while (xml.readNext() && !xml.atEnd() && xml.name() != "phonemeGroup") {
                    if (xml.name() == "id") {
                        group->setId(xml.readElementText());
                    }
                    if (xml.name() == "title") {
                        group->setTitle(xml.readElementText());
                    }
                    if (xml.name() == "description") {
                        group->setDescription(xml.readElementText());
                    }
                    if (xml.name() == "phoneme") {
                        std::unique_ptr<Phoneme> phoneme = std::unique_ptr<Phoneme>(new Phoneme);
                        while (xml.readNext() && !xml.atEnd() && xml.name() != "phoneme") {
                            if (xml.name() == "id") {
                                phoneme->setId(xml.readElementText());
                            }
                            if (xml.name() == "title") {
                                phoneme->setTitle(xml.readElementText());
                            }
                        }
                        group->addPhoneme(std::move(phoneme));
                    }
                }
                language->m_phonemeGroups.append(group);
            }
        }
        if (xml.hasError()) {
            qCritical() << "Error occurred when reading Language XML file:" << file.toLocalFile();
        }
    }
    xml.clear();
    handle.close();

    return language;
}

Language::Language()
    : ILanguage()
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
