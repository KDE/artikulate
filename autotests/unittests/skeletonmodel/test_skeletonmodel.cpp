/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "test_skeletonmodel.h"
#include "../mocks/editablecoursestub.h"
#include "../mocks/editablerepositorystub.h"
#include "../mocks/languagestub.h"
#include "src/core/icourse.h"
#include "src/core/language.h"
#include "src/org/kde/artikulate/skeletonmodel.h"
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
