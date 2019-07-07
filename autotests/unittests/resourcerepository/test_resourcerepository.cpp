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

#include "test_resourcerepository.h"
#include <QTest>
#include <QSignalSpy>
#include <QObject>
#include "src/core/resourcerepository.h"
#include "src/core/language.h"

void TestResourceRepository::init()
{
    // check that test data is deployed at the expected location
    QVERIFY(QFile::exists("data/courses/de/de.xml"));
    QVERIFY(QFile::exists("data/courses/fr/fr.xml"));
}

void TestResourceRepository::cleanup()
{
    // TODO cleanup after test run
}

void TestResourceRepository::createRepository()
{
    ResourceRepository repository(QUrl::fromLocalFile("data/courses/"));
    QCOMPARE(repository.storageLocation(), "data/courses/");
    repository.reloadCourses();
    QCOMPARE(repository.courses().count(), 2);
}

void TestResourceRepository::iResourceRepositoryCompatability()
{
    ResourceRepository repository(QUrl::fromLocalFile("data/courses/"));
    IResourceRepository *interface = &repository;

    QCOMPARE(interface->storageLocation(), "data/courses/");
    QVERIFY(interface->languages().count() > 0);
    QCOMPARE(interface->courses().count(), 0);

    // test adding
    QSignalSpy spyAboutToBeAdded(dynamic_cast<QObject *>(interface), SIGNAL(courseAboutToBeAdded(std::shared_ptr<ICourse>, int)));
    QSignalSpy spyAdded(dynamic_cast<QObject *>(interface), SIGNAL(courseAdded()));
    QCOMPARE(spyAboutToBeAdded.count(), 0);
    QCOMPARE(spyAdded.count(), 0);
    repository.reloadCourses();
    QCOMPARE(interface->courses().count(), 2);
    QCOMPARE(spyAboutToBeAdded.count(), 2);
    QCOMPARE(spyAdded.count(), 2);

    // test removal
    // note: repository does not provide removal of courses, yet

    // test access of courses grouped by language
    auto languages = interface->languages();
    std::shared_ptr<Language> german;
    for (auto language : interface->languages()) {
        if (language->id() == "de") {
            german = language;
            break;
        }
    }
    QVERIFY(german != nullptr); // ensure that German language was found
    QCOMPARE(interface->courses(german->id()).count(), 1); // there is exactly one German course
    QCOMPARE(interface->courses(nullptr).count(), 2); // all courses in total are 2
}

QTEST_GUILESS_MAIN(TestResourceRepository)
