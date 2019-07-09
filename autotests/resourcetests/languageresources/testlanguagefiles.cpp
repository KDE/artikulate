/*
 *  Copyright 2013  Oindrila Gupta <oindrila.gupta92@gmail.com>
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

#include "testlanguagefiles.h"
#include "core/language.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"
#include "core/resources/courseparser.h"

#include <QTest>
#include <QDirIterator>
#include <QDebug>
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
