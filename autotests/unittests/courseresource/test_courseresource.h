/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef TESTCOURSERESOURCE_H
#define TESTCOURSERESOURCE_H

#include <QObject>
#include <QVariant>

class TestCourseResource : public QObject
{
    Q_OBJECT

public:
    TestCourseResource();

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
    void loadCourseResource();

    /**
     * @brief Test simple loading of course resource XML file and skip all incomplete units/phrases
     */
    void loadCourseResourceSkipIncomplete();

    /**
     * @brief Test handling of unit insertions (specifically, the signals)
     */
    void unitAddAndRemoveHandling();

    /**
     * @brief Test of all course property changes except unit handling
     */
    void coursePropertyChanges();

private:
    bool m_systemUseCourseRepositoryValue;
};

#endif
