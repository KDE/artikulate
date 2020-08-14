/*
 *  SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "test_resourcerepository.h"
#include "src/core/language.h"
#include "src/core/resourcerepository.h"
#include <QObject>
#include <QSignalSpy>
#include <QTest>

TestResourceRepository::TestResourceRepository()
    : m_repositoryLocation(QUrl::fromLocalFile(qApp->applicationDirPath() + "/../autotests/unittests/data/"))
{
}

void TestResourceRepository::init()
{
    // check that test data is deployed at the expected location
    QVERIFY(QFile::exists(m_repositoryLocation.toLocalFile() + "/courses/de/de.xml"));
    QVERIFY(QFile::exists(m_repositoryLocation.toLocalFile() + "/courses/fr/fr.xml"));
}

void TestResourceRepository::cleanup()
{
    // TODO cleanup after test run
}

void TestResourceRepository::createRepository()
{
    ResourceRepository repository(QUrl::fromLocalFile(m_repositoryLocation.toLocalFile() + "/courses/"));
    QCOMPARE(repository.storageLocation(), QUrl::fromLocalFile(m_repositoryLocation.toLocalFile() + "/courses/"));
    repository.reloadCourses();
    QCOMPARE(repository.courses().count(), 2);
}

void TestResourceRepository::iResourceRepositoryCompatability()
{
    ResourceRepository repository(QUrl::fromLocalFile(m_repositoryLocation.toLocalFile() + "/courses/"));
    IResourceRepository *interface = &repository;

    QCOMPARE(interface->storageLocation(), QUrl::fromLocalFile(m_repositoryLocation.toLocalFile() + "/courses/"));
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
}

QTEST_GUILESS_MAIN(TestResourceRepository)
