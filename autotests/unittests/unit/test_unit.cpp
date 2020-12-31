/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "test_unit.h"
#include "../mocks/languagestub.h"
#include "core/language.h"
#include "core/phonemegroup.h"
#include "core/phrase.h"
#include "core/unit.h"
#include "resourcerepositorystub.h"
#include <QDebug>
#include <QSignalSpy>

TestUnit::TestUnit() = default;

void TestUnit::init()
{
    qRegisterMetaType<std::shared_ptr<IPhrase>>("std::shared_ptr<IPhrase>");
}

void TestUnit::cleanup()
{
}

void TestUnit::unitPropertyChanges()
{
    auto unit = Unit::create();
    QVERIFY(unit);

    {
        QSignalSpy spy(unit.get(), &Unit::idChanged);
        QString id("testId");
        unit->setId(id);
        QCOMPARE(unit->id(), id);
        QCOMPARE(spy.count(), 1);
    }
    {
        QString foreignId("foreignId");
        unit->setForeignId(foreignId);
        QCOMPARE(unit->foreignId(), foreignId);
    }
    {
        QSignalSpy spy(unit.get(), &Unit::titleChanged);
        QString title("title");
        unit->setTitle(title);
        QCOMPARE(unit->title(), title);
        QCOMPARE(spy.count(), 1);
    }
}

void TestUnit::addAndRemovePhrases()
{
    auto unit = Unit::create();
    QString unitId {"unitId"};
    unit->setId(unitId);
    QVERIFY(unit);

    auto phraseA = Phrase::create();
    QString phraseIdA {"phraseIdA"};
    phraseA->setId(phraseIdA);

    auto phraseB = Phrase::create();
    QString phraseIdB {"phraseIdB"};
    phraseB->setId(phraseIdB);

    {
        QSignalSpy aboutSpy(unit.get(), &Unit::phraseAboutToBeAdded);
        QSignalSpy addedSpy(unit.get(), &Unit::phraseAdded);
        QCOMPARE(unit->phrases().count(), 0);
        unit->addPhrase(phraseA, 0);
        QCOMPARE(unit->phrases().count(), 1);
        QCOMPARE(aboutSpy.count(), 1);
        auto parameters = aboutSpy.takeFirst();
        // TODO check object parameter
        QCOMPARE(parameters.at(1).toInt(), 0);
        QCOMPARE(addedSpy.count(), 1);
        QCOMPARE(phraseA->unit()->id(), unitId);
    }

    { // prepend second phrase
        QSignalSpy aboutSpy(unit.get(), &Unit::phraseAboutToBeAdded);
        QSignalSpy addedSpy(unit.get(), &Unit::phraseAdded);
        QCOMPARE(unit->phrases().count(), 1);
        unit->addPhrase(phraseB, 0);
        QCOMPARE(unit->phrases().count(), 2);
        QCOMPARE(aboutSpy.count(), 1);
        auto parameters = aboutSpy.takeFirst();
        // TODO check object parameter
        QCOMPARE(parameters.at(1).toInt(), 0);
        QCOMPARE(addedSpy.count(), 1);
        QCOMPARE(phraseB->unit()->id(), unitId);
    }

    {
        QSignalSpy aboutSpy(unit.get(), &Unit::phraseAboutToBeRemoved);
        QSignalSpy removedSpy(unit.get(), &Unit::phraseRemoved);
        QCOMPARE(unit->phrases().count(), 2);
        unit->removePhrase(phraseA);
        QCOMPARE(unit->phrases().count(), 1);
        QCOMPARE(aboutSpy.count(), 1);
        QCOMPARE(removedSpy.count(), 1);
    }
}

QTEST_GUILESS_MAIN(TestUnit)
