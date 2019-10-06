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
