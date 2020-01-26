/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef TEST_SKELETONMODEL_H
#define TEST_SKELETONMODEL_H

#include <QObject>

class TestSkeletonModel : public QObject
{
    Q_OBJECT

public:
    TestSkeletonModel() = default;

private Q_SLOTS:
    /**
     * Called before every test case.
     */
    void init();

    /**
     * Called after every test case.
     */
    void cleanup();

    /**
     * @brief Test course model initialization from a resource repository stub
     */
    void testInit();

    /**
     * @brief Test add/remove signal propagation as emitted from resource repository stub
     */
    void testAddRemoveOperations();

    /**
     * @brief Test data changed signal emit
     */
    void testDataChangedSignals();
};

#endif
