/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
