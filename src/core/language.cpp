/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "language.h"
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
            if (xml.name() == QLatin1String("id")) {
                language->setId(xml.readElementText());
            }
            if (xml.name() == QLatin1String("title")) {
                language->setTitle(xml.readElementText());
            }
            if (xml.name() == QLatin1String("i18nTitle")) {
                language->seti18nTitle(xml.readElementText());
            }
            if (xml.name() == QLatin1String("phonemeGroup")) {
                std::shared_ptr<PhonemeGroup> group = std::shared_ptr<PhonemeGroup>(new PhonemeGroup);
                while (xml.readNext() && !xml.atEnd() && xml.name() != QLatin1String("phonemeGroup")) {
                    if (xml.name() == QLatin1String("id")) {
                        group->setId(xml.readElementText());
                    }
                    if (xml.name() == QLatin1String("title")) {
                        group->setTitle(xml.readElementText());
                    }
                    if (xml.name() == QLatin1String("description")) {
                        group->setDescription(xml.readElementText());
                    }
                    if (xml.name() == QLatin1String("phoneme")) {
                        std::unique_ptr<Phoneme> phoneme = std::unique_ptr<Phoneme>(new Phoneme);
                        while (xml.readNext() && !xml.atEnd() && xml.name() != QLatin1String("phoneme")) {
                            if (xml.name() == QLatin1String("id")) {
                                phoneme->setId(xml.readElementText());
                            }
                            if (xml.name() == QLatin1String("title")) {
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
        Q_EMIT idChanged();
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
        Q_EMIT titleChanged();
    }
}

QString Language::i18nTitle() const
{
    return i18n(m_i18nTitle.toUtf8().data());
}

void Language::seti18nTitle(const QString &title)
{
    if (m_i18nTitle == title) {
        return;
    }
    m_i18nTitle = title;
    Q_EMIT i18nTitleChanged();
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
