/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef TESTEDITABLECOURSERESOURCE_H
#define TESTEDITABLECOURSERESOURCE_H

#include <QObject>
#include <QVariant>

class TestEditableCourseResource : public QObject
{
    Q_OBJECT

public:
    TestEditableCourseResource();

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
     * @brief Test simple loading of course resource XML file
     */
    void loadCourseResource();

    /**
     * @brief Test handling of unit insertions (specifically, the signals)
     */
    void unitAddAndRemoveHandling();

    /**
     * @brief Test handling of phrase insertions (specifically, the signals)
     */
    void phraseAddAndRemoveHandling();

    /**
     * @brief Test of all course property changes except unit handling
     */
    void coursePropertyChanges();

    /**
     * Test if serialization of unserialized file gives original file.
     */
    void fileLoadSaveCompleteness();

    /**
     * Test if the modified status is correctly set.
     */
    void modifiedStatus();

    /**
     * Test correct update of course from a skeleton
     */
    void skeletonUpdate();
};

#endif
