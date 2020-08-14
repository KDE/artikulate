/*
 *  SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef TESTXSDSCHEMES_H
#define TESTXSDSCHEMES_H

#include <QObject>

class TestXsdSchemes : public QObject
{
    Q_OBJECT

public:
    TestXsdSchemes();

private slots:
    /**
     * Called before every test case.
     */
    void init();

    /**
     * Called after every test case.
     */
    void cleanup();

    /**
     * @brief Test if language XSD specification is valid.
     */
    void languageSchemeValidationTest();

    /**
     * @brief Test if course XSD specification is valid.
     */
    void courseSchemeValidationTest();

    /**
     * @brief Test if course XSD specification is valid.
     */
    void schemeValidationTest();
};

#endif
