/*
 *  SPDX-FileCopyrightText: 2013 Oindrila Gupta <oindrila.gupta92@gmail.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "testlanguagefiles.h"
#include "core/language.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"
#include "core/phrase.h"
#include "core/resources/courseparser.h"
#include "core/unit.h"

#include <QDebug>
#include <QDirIterator>
#include <QTest>
#include <QUrl>

TestLanguageFiles::TestLanguageFiles()
{
}

void TestLanguageFiles::init()
{
    // TODO initialization of test case
}

void TestLanguageFiles::cleanup()
{
    // TODO cleanup after test run
}

void TestLanguageFiles::loadGerman()
{
    QUrl file = QUrl::fromLocalFile(":/artikulate/languages/de.xml");
    auto language = Language::create(file);

    QCOMPARE(language->id(), "de");
    QCOMPARE(language->title(), "Deutsch");
    QCOMPARE(language->i18nTitle(), "German");

    std::shared_ptr<PhonemeGroup> group;
    for (auto iter : language->phonemeGroups()) {
        if (iter->id() == "monophthonge") {
            group = iter;
            break;
        }
    }
    QVERIFY(group);
    QCOMPARE(group->id(), "monophthonge");
    QCOMPARE(group->title(), "Vokalsystem Monophthonge");
    QCOMPARE(group->description(), "Monophthonge");

    std::shared_ptr<Phoneme> phoneme;
    for (auto iter : language->phonemes()) {
        if (iter->id() == "a") {
            phoneme = iter;
            break;
        }
    }
    QVERIFY(phoneme);
    QCOMPARE(phoneme->id(), "a");
    QCOMPARE(phoneme->title(), "[a] Kamm");
}

void TestLanguageFiles::checkIdUniqueness()
{
    QDirIterator iter(QDir(":/artikulate/languages/"));
    while (iter.hasNext()) {
        const QString &file = iter.next();
        qDebug() << "File being parsed: " << file;
        QStringList idList;

        auto language = Language::create(QUrl::fromLocalFile(file));
        for (auto phoneme : language->phonemes()) {
            QVERIFY2(!idList.contains(phoneme->id()), "Phoneme ID used more than once in the tested file");
            idList.append(phoneme->id());
        }
    }
}

QTEST_GUILESS_MAIN(TestLanguageFiles)
