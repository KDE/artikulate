/*
    SPDX-FileCopyrightText: 2013 Oindrila Gupta <oindrila.gupta92@gmail.com>
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef TESTLANGUAGEFILES_H
#define TESTLANGUAGEFILES_H

#include <QObject>

class TestLanguageFiles : public QObject
{
    Q_OBJECT

public:
    TestLanguageFiles();

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
     * Test language parser by loading German
     */
    void loadGerman();

    /**
     * Test if id of each phoneme is unique in every language file.
     */
    void checkIdUniqueness();
};

#endif
