/*
 *  SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef APPLICATION_H
#define APPLICATION_H

#include "artikulatecore_export.h"
#include <QApplication>

class IResourceRepository;
class IEditableRepository;
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
    IResourceRepository *resourceRepository() const;

    /**
     * @brief getter for global editable repository
     * @return the repository
     */
    IEditableRepository *editableRepository() const;

private:
    void registerQmlTypes();
    IResourceRepository *m_resourceRepository {nullptr};
};

#endif
