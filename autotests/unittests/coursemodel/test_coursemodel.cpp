/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "test_coursemodel.h"
#include "../mocks/coursestub.h"
#include "../mocks/languagestub.h"
#include "../mocks/resourcerepositorystub.h"
#include "src/core/icourse.h"
#include "src/core/language.h"
#include "src/org/kde/artikulate/coursemodel.h"

#include <QSignalSpy>
#include <QTest>

void TestCourseModel::init()
{
    // TODO initialization of test case
}

void TestCourseModel::cleanup()
{
    // TODO cleanup after test run
}

void TestCourseModel::testInit()
{
    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    std::vector<std::shared_ptr<ILanguage>> languages;
    languages.push_back(language);
    auto course = CourseStub::create(language, QVector<std::shared_ptr<Unit>>({}));
    ResourceRepositoryStub repository(languages, {course});

    // test initialization
    CourseModel model(&repository);
    QVERIFY(model.resourceRepository() == &repository);
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.course(0).value<QObject *>(), course.get());
}

void TestCourseModel::testAddRemoveOperations()
{
    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    std::vector<std::shared_ptr<ILanguage>> languages;
    languages.push_back(language);
    ResourceRepositoryStub repository(languages, {});

    // test initialization
    CourseModel model(&repository);
    QVERIFY(model.resourceRepository() == &repository);
    QCOMPARE(model.rowCount(), 0);

    auto course = CourseStub::create(language, QVector<std::shared_ptr<Unit>>({}));

    { // add course
        QSignalSpy spyAboutToBeAdded(&repository, SIGNAL(courseAboutToBeAdded(std::shared_ptr<ICourse>, int)));
        QSignalSpy spyAdded(&repository, SIGNAL(courseAdded()));
        QCOMPARE(spyAboutToBeAdded.count(), 0);
        QCOMPARE(spyAdded.count(), 0);
        repository.appendCourse(course);
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.course(0).value<QObject *>(), course.get());
        QCOMPARE(spyAboutToBeAdded.count(), 1);
        QCOMPARE(spyAdded.count(), 1);
    }

    { // remove course
        QSignalSpy spyAboutToBeRemoved(&repository, SIGNAL(courseAboutToBeRemoved(int)));
        QSignalSpy spyRemoved(&repository, SIGNAL(courseRemoved()));
        QCOMPARE(spyAboutToBeRemoved.count(), 0);
        QCOMPARE(spyRemoved.count(), 0);
        repository.removeCourse(course);
        QCOMPARE(model.rowCount(), 0);
        QCOMPARE(spyAboutToBeRemoved.count(), 1);
        QCOMPARE(spyRemoved.count(), 1);
    }
}

void TestCourseModel::testDataChangedSignals()
{
    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    std::vector<std::shared_ptr<ILanguage>> languages;
    languages.push_back(language);
    auto course = CourseStub::create(language, QVector<std::shared_ptr<Unit>>({}));
    ResourceRepositoryStub repository(languages, {course});

    // test initialization
    CourseModel model(&repository);
    QVERIFY(model.resourceRepository() == &repository);
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.course(0).value<QObject *>(), course.get());

    { // test adding of connections
        QSignalSpy spyUpdate(&model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int>)));
        QCOMPARE(spyUpdate.count(), 0);
        std::static_pointer_cast<CourseStub>(course)->setTitle("TitleSwitched");
        QCOMPARE(spyUpdate.count(), 1);
    }

    { // test removal of connections
        QSignalSpy spyUpdate(&model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int>)));
        QCOMPARE(spyUpdate.count(), 0);
        repository.removeCourse(course);
        std::static_pointer_cast<CourseStub>(course)->setTitle("TitleSwitchedAgain");
        QCOMPARE(spyUpdate.count(), 0);
    }
}

QTEST_GUILESS_MAIN(TestCourseModel)
