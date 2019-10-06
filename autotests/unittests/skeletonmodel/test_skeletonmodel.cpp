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

#include "test_skeletonmodel.h"
#include "../mocks/editablecoursestub.h"
#include "../mocks/editablerepositorystub.h"
#include "../mocks/languagestub.h"
#include "src/core/icourse.h"
#include "src/core/language.h"
#include "src/models/skeletonmodel.h"

#include <QSignalSpy>
#include <QTest>

void TestSkeletonModel::init()
{
    // no initialization of test case
}

void TestSkeletonModel::cleanup()
{
    // no cleanup after test run
}

void TestSkeletonModel::testInit()
{
    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    std::vector<std::shared_ptr<ILanguage>> languages;
    languages.push_back(language);
    auto skeleton = EditableCourseStub::create(language, QVector<std::shared_ptr<Unit>>({}));
    EditableRepositoryStub repository(languages, {skeleton}, {});

    // test initialization
    SkeletonModel model(&repository);
    QVERIFY(model.resourceRepository() == &repository);
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.skeleton(0).value<QObject *>(), skeleton.get());
}

void TestSkeletonModel::testAddRemoveOperations()
{
    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    std::vector<std::shared_ptr<ILanguage>> languages;
    languages.push_back(language);
    EditableRepositoryStub repository(languages, {}, {});

    // test initialization
    SkeletonModel model(&repository);
    QVERIFY(model.resourceRepository() == &repository);
    QCOMPARE(model.rowCount(), 0);

    auto skeleton = EditableCourseStub::create(language, QVector<std::shared_ptr<Unit>>({}));

    { // add skeleton
        QSignalSpy spyAboutToBeAdded(&repository, SIGNAL(skeletonAboutToBeAdded(std::shared_ptr<IEditableCourse>, int)));
        QSignalSpy spyAdded(&repository, SIGNAL(skeletonAdded()));
        QCOMPARE(spyAboutToBeAdded.count(), 0);
        QCOMPARE(spyAdded.count(), 0);
        repository.appendSkeleton(skeleton);
        QCOMPARE(model.rowCount(), 1);
        QCOMPARE(model.skeleton(0).value<QObject *>(), skeleton.get());
        QCOMPARE(spyAboutToBeAdded.count(), 1);
        QCOMPARE(spyAdded.count(), 1);
    }

    { // remove skeleton
        QSignalSpy spyAboutToBeRemoved(&repository, SIGNAL(skeletonAboutToBeRemoved(int)));
        QSignalSpy spyRemoved(&repository, SIGNAL(skeletonRemoved()));
        QCOMPARE(spyAboutToBeRemoved.count(), 0);
        QCOMPARE(spyRemoved.count(), 0);
        repository.removeSkeleton(skeleton);
        QCOMPARE(model.rowCount(), 0);
        QCOMPARE(spyAboutToBeRemoved.count(), 1);
        QCOMPARE(spyRemoved.count(), 1);
    }
}

void TestSkeletonModel::testDataChangedSignals()
{
    // boilerplate
    std::shared_ptr<ILanguage> language(new LanguageStub("de"));
    std::vector<std::shared_ptr<ILanguage>> languages;
    languages.push_back(language);
    auto skeleton = EditableCourseStub::create(language, QVector<std::shared_ptr<Unit>>({}));
    EditableRepositoryStub repository(languages, {skeleton}, {});

    // test initialization
    SkeletonModel model(&repository);
    QVERIFY(model.resourceRepository() == &repository);
    QCOMPARE(model.rowCount(), 1);
    QCOMPARE(model.skeleton(0).value<QObject *>(), skeleton.get());

    { // test adding of connections
        QSignalSpy spyUpdate(&model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int>)));
        QCOMPARE(spyUpdate.count(), 0);
        std::static_pointer_cast<EditableCourseStub>(skeleton)->setTitle("TitleSwitched");
        QCOMPARE(spyUpdate.count(), 1);
    }

    { // test removal of connections
        QSignalSpy spyUpdate(&model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int>)));
        QCOMPARE(spyUpdate.count(), 0);
        repository.removeSkeleton(skeleton);
        std::static_pointer_cast<EditableCourseStub>(skeleton)->setTitle("TitleSwitchedAgain");
        QCOMPARE(spyUpdate.count(), 0);
    }
}

QTEST_GUILESS_MAIN(TestSkeletonModel)
