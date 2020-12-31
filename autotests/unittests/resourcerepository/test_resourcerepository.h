/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef TEST_RESOURCEREPOSITORY_H
#define TEST_RESOURCEREPOSITORY_H

#include <QObject>
#include <QUrl>

class TestResourceRepository : public QObject
{
    Q_OBJECT

public:
    TestResourceRepository();

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
     * Create and load repository with simple testdata
     */
    void createRepository();

    /**
     * @brief integration test for IResourceRepository interface
     * Test expectations of the IResourceRepository interface.
     */
    void iResourceRepositoryCompatability();

private:
    QUrl m_repositoryLocation;
};

#endif
