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
