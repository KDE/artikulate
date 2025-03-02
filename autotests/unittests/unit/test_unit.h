/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef TESTUNIT_H
#define TESTUNIT_H

#include <QObject>
#include <QTest>
#include <QVariant>

class TestUnit : public QObject
{
    Q_OBJECT

public:
    TestUnit();

private Q_SLOTS:
    /**
     * @brief Called before every test case.
     */
    void init();

    /**
     * @brief Called after every test case.
     */
    void cleanup();

    /**
     * @brief Test of all unit property changes except unit handling
     */
    void unitPropertyChanges();

    /**
     * @brief Test of phrase add/remove operations
     */
    void addAndRemovePhrases();
};

#endif
