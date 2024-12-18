/*
    SPDX-FileCopyrightText: 2024 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef TEST_DRAWERCOURSETREEMODEL_H
#define TEST_DRAWERCOURSETREEMODEL_H

#include <QObject>

class TestDrawerCourseTreeModel : public QObject
{
    Q_OBJECT

public:
    TestDrawerCourseTreeModel() = default;

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
     * @brief Construct and destruct training session without units
     */
    void createTrainingSessionWithoutUnits();

    /**
     * @brief Construct and destruct training session simple units
     */
    void createTrainingSessionWithBasicUnits();

    /**
     * @brief Construct training session and check that phrases without sound file paths are skipped
     */
    void createTrainingSessionWithEmptySounds();

    /**
     * @brief Construct and destruct training session only empty units
     */
    void createTrainingSessionWithEmptyUnits();

    /**
     * @brief Test for all iterator functionality
     */
    void iterateCourse();
};

#endif
