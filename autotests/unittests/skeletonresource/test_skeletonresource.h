/*
 *  SPDX-FileCopyrightText: 2013-2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef TESTSKELETONRESOURCE_H
#define TESTSKELETONRESOURCE_H

#include <QObject>
#include <QVariant>

class TestSkeletonResource : public QObject
{
    Q_OBJECT

public:
    TestSkeletonResource();

private slots:
    /**
     * @brief Called before every test case.
     */
    void init();

    /**
     * @brief Called after every test case.
     */
    void cleanup();

    /**
     * @brief Test simple loading of course resource XML file
     */
    void loadSkeletonResource();

    /**
     * @brief Test handling of unit insertions (specifically, the signals)
     */
    void unitAddAndRemoveHandling();

    /**
     * @brief Test of all course property changes except unit handling
     */
    void coursePropertyChanges();

    /**
     * Test if serialization of unserialized file gives original file.
     * TODO this is a test by only string equality and should improved to test on a data level
     */
    void fileLoadSaveCompleteness();

private:
    bool m_systemUseCourseRepositoryValue;
};

#endif
