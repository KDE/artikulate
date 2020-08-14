/*
 *  SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef TEST_IRESOURCEREPOSITORY_H
#define TEST_IRESOURCEREPOSITORY_H

#include <QObject>
#include <QUrl>

class IResourceRepository;

class TestIResourceRepository : public QObject
{
    Q_OBJECT

public:
    TestIResourceRepository();

private Q_SLOTS:
    /**
     * Called before every test case.
     */
    void init();

    /**
     * @brief integration test of ResourceRepository for IResourceRepository interface contract
     * Test that expectations of the IResourceRepository interface are implemented by
     * ResourceRepository class.
     */
    void resourceRepository();

    /**
     * @brief integration test of ContributorRepository for IResourceRepository interface contract
     * Test that expectations of the IResourceRepository interface are implemented by
     * ContributorRepository class.
     */
    void contributorRepository();

private:
    void performInterfaceTests(IResourceRepository *repository);
    QUrl m_repositoryLocation;
};

#endif
