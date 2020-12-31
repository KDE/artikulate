/*
    SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef TESTLEARNERSTORAGE_H
#define TESTLEARNERSTORAGE_H

#include <QObject>
#include <QScopedPointer>
#include <QTemporaryFile>

namespace LearnerProfile
{
class Storage;
}

class TestLearnerStorage : public QObject
{
    Q_OBJECT

public:
    TestLearnerStorage();

private Q_SLOTS:
    /**
     * Called before every test case.
     */
    void init();

    /**
     * Called after every test case.
     */
    void cleanup();

    void testLearnerStorage();
    void testProgressLogStorage();
    void testProgressValueStorage();

private:
    QScopedPointer<LearnerProfile::Storage> m_storage;
    QTemporaryFile m_db;
};

#endif
