/*
 *  SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef TEST_TRAININGSESSION_H
#define TEST_TRAININGSESSION_H

#include <QObject>

class TestTrainingSession : public QObject
{
    Q_OBJECT

public:
    TestTrainingSession() = default;

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
     * @brief Construct training session and check that phrases without sound file paths are skipped
     */
    void createTrainingSessionWithEmptySounds();

    /**
     * @brief Construct and destruct training session only empty units
     */
    void createTrainingSessionWithEmptyUnits();

    /**
     * @brief Construct and destruct training session only non-empty units
     */
    void createTrainingSessionWithUnitsAndPhrases();

    /**
     * @brief Test for all iterator functionality
     */
    void iterateCourse();
};

#endif
