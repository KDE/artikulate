/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef TEST_EDITORSESSION_H
#define TEST_EDITORSESSION_H

#include <QObject>

class TestEditorSession : public QObject
{
    Q_OBJECT

public:
    TestEditorSession() = default;

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
     * @brief Construct and destruct editor session and test initial values
     */
    void createEditorSession();

    /**
     * @brief Test switching behavior for courses without skeleton.
     */
    void nonSkeletonSwitchingBehavior();

    /**
     * @brief Test handling of skeletons and respective course switching
     */
    void skeletonSwitchingBehavior();

    /**
     * @brief Test for all iterator functionality
     */
    void iterateCourse();

    /**
     * @brief Test that the actions are correctly updated when course changes;
     */
    void updateActionsBehavior();
};

#endif
