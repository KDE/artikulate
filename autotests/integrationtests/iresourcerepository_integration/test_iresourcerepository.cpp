/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "test_iresourcerepository.h"
#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include "../src/settings.h"
#include "core/contributorrepository.h"
#include "core/icourse.h"
#include "core/language.h"
#include "core/resourcerepository.h"
#include "core/unit.h"

TestIResourceRepository::TestIResourceRepository()
    : m_repositoryLocation(QUrl::fromLocalFile(qApp->applicationDirPath() + "/../autotests/integrationtests/data/"))
{
    qDebug() << "Running test with repository root:" << m_repositoryLocation;
}

void TestIResourceRepository::init()
{
    // check that test data is deployed at the expected location
    QVERIFY(QFile::exists(m_repositoryLocation.toLocalFile() + "/courses/de/de.xml"));
    QVERIFY(QFile::exists(m_repositoryLocation.toLocalFile() + "/courses/fr/fr.xml"));
}

void TestIResourceRepository::resourceRepository()
{
    ResourceRepository repository(QUrl::fromLocalFile(m_repositoryLocation.toLocalFile() + "/courses/"));
    QCOMPARE(repository.storageLocation(), QUrl::fromLocalFile(m_repositoryLocation.toLocalFile() + "/courses/"));
    performInterfaceTests(&repository);
}

void TestIResourceRepository::contributorRepository()
{
    ContributorRepository repository(QUrl::fromLocalFile(m_repositoryLocation.toLocalFile() + "/contributorrepository/")); // contributor repository requires subdirectory "courses"
    QCOMPARE(repository.storageLocation(), QUrl::fromLocalFile(m_repositoryLocation.toLocalFile() + "/contributorrepository/"));
    performInterfaceTests(&repository);
}

void TestIResourceRepository::performInterfaceTests(IResourceRepository *interface)
{
    QVERIFY(interface->languages().count() > 0); // automatically load languages
    QCOMPARE(interface->courses().count(), 0);   // load courses only on demand

    // test adding
    QSignalSpy spyAboutToBeAdded(dynamic_cast<QObject *>(interface), SIGNAL(courseAboutToBeAdded(std::shared_ptr<ICourse>, int)));
    QSignalSpy spyAdded(dynamic_cast<QObject *>(interface), SIGNAL(courseAdded()));
    QCOMPARE(spyAboutToBeAdded.count(), 0);
    QCOMPARE(spyAdded.count(), 0);
    interface->reloadCourses(); // initial loading of courses
    QCOMPARE(interface->courses().count(), 2);
    QCOMPARE(spyAboutToBeAdded.count(), 2);
    QCOMPARE(spyAdded.count(), 2);

    // test reloading of courses
    interface->reloadCourses(); // initial loading of courses
    QCOMPARE(interface->courses().count(), 2);

    // test removal
    // note: repository does not provide removal of courses, yet

    // test access of courses grouped by language
    auto languages = interface->languages();
    std::shared_ptr<ILanguage> german;
    for (auto language : interface->languages()) {
        if (language->id() == "de") {
            german = language;
            break;
        }
    }
    QVERIFY(german != nullptr);                            // ensure that German language was found
    QCOMPARE(interface->courses(german->id()).count(), 1); // there is exactly one German course
    QCOMPARE(interface->courses(nullptr).count(), 2);      // all courses in total are 2
    QVERIFY(interface->courses().first()->units().size() > 0);
}

QTEST_GUILESS_MAIN(TestIResourceRepository)
