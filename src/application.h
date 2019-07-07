/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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


#ifndef APPLICATION_H
#define APPLICATION_H

#include "artikulatecore_export.h"
#include <QApplication>

class IResourceRepository;
class Application;

#if defined(artikulateApp)
#undef artikulateApp
#endif
#define artikulateApp (static_cast<Application *>(QCoreApplication::instance()))

class ARTIKULATECORE_EXPORT Application : public QApplication
{
    Q_OBJECT

public:
    explicit Application(int &argc, char **argv);

    /**
     * @brief install global course data repository to application
     * @param repository the concrete resource repository to install
     */
    void installResourceRepository(IResourceRepository *repository);

    /**
     * @brief getter for global resource repository
     * @return the repository
     */
    IResourceRepository * resourceRepository() const;

private:
    void registerQmlTypes();
    IResourceRepository *m_resourceRepository{ nullptr };
};

#endif
