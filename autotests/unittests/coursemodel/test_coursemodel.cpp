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

#include "test_coursemodel.h"
#include "src/core/icourse.h"
#include "src/core/language.h"
#include "src/models/coursemodel.h"
#include "../mocks/resourcerepositorystub.h"
#include "../mocks/coursestub.h"

#include <QTest>
#include <QSignalSpy>

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
    std::shared_ptr<Language> language(new Language);
    language->setId("de");
    std::vector<std::shared_ptr<Language>> languages;
    languages.push_back(language);
    auto course = CourseStub::create(language, QVector<std::shared_ptr<Unit>>({}));
    ResourceRepositoryStub repository(languages, {course});

    // test initialization
    CourseModel model(&repository);
    QVERIFY(model.resourceRepository() == &repository);
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.course(0).value<QObject*>(), course.get());
}

void TestCourseModel::testAddRemoveOperations()
{
    // boilerplate
    std::shared_ptr<Language> language(new Language);
    language->setId("de");
    std::vector<std::shared_ptr<Language>> languages;
    languages.push_back(language);
    ResourceRepositoryStub repository(languages, {});

    // test initialization
    CourseModel model(&repository);
    QVERIFY(model.resourceRepository() == &repository);
    QCOMPARE(model.rowCount(), 0);

    auto course = CourseStub::create(language, QVector<std::shared_ptr<Unit>>({}));

    { // add course
        QSignalSpy spyAboutToBeAdded(&repository, SIGNAL(courseAboutToBeAdded(std::shared_ptr<ICourse>,int)));
        QSignalSpy spyAdded(&repository, SIGNAL(courseAdded()));
        QCOMPARE(spyAboutToBeAdded.count(), 0);
        QCOMPARE(spyAdded.count(), 0);
        repository.appendCourse(course);
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.course(0).value<QObject*>(), course.get());
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
    std::shared_ptr<Language> language(new Language);
    language->setId("de");
    std::vector<std::shared_ptr<Language>> languages;
    languages.push_back(language);
    auto course = CourseStub::create(language, QVector<std::shared_ptr<Unit>>({}));
    ResourceRepositoryStub repository(languages, {course});

    // test initialization
    CourseModel model(&repository);
    QVERIFY(model.resourceRepository() == &repository);
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.course(0).value<QObject*>(), course.get());

    { // test adding of connections
        QSignalSpy spyUpdate(&model, SIGNAL(dataChanged(const QModelIndex &,const QModelIndex &, const QVector<int>)));
        QCOMPARE(spyUpdate.count(), 0);
        std::static_pointer_cast<CourseStub>(course)->setTitle("TitleSwitched");
        QCOMPARE(spyUpdate.count(), 1);
    }

    { // test removal of connections
        QSignalSpy spyUpdate(&model, SIGNAL(dataChanged(const QModelIndex &,const QModelIndex &, const QVector<int>)));
        QCOMPARE(spyUpdate.count(), 0);
        repository.removeCourse(course);
        std::static_pointer_cast<CourseStub>(course)->setTitle("TitleSwitchedAgain");
//        QCOMPARE(spyUpdate.count(), 0);
    }
}

QTEST_GUILESS_MAIN(TestCourseModel)
