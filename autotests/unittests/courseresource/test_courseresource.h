/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License or (at your option) version 3 or any later version
 *  accepted by the membership of KDE e.V. (or its successor approved
 *  by the membership of KDE e.V.), which shall act as a proxy
 *  defined in Section 14 of version 3 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
