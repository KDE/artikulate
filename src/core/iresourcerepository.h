/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef IRESOURCEREPOSITORY_H
#define IRESOURCEREPOSITORY_H

#include "artikulatecore_export.h"
#include <QObject>
#include <memory>

class ICourse;
class ILanguage;

/**
 * \class IResourceRepository
 * This interface provides a generic interface that provides just the methods and signals needed
 * to integrade a repository into the training part of Artikulate.
 */
class ARTIKULATECORE_EXPORT IResourceRepository : public QObject
{
    Q_OBJECT
public:
    virtual ~IResourceRepository() = default;

    /**
     * \return path to working repository, if one is set
     */
    virtual QUrl storageLocation() const = 0;

    /**
     * @return list of all loaded courses
     */
    virtual QVector<std::shared_ptr<ICourse>> courses() const = 0;

    /**
     * @param language to use for filtering
     * @return list of all loaded courses filtered by the named language
     */
    virtual QVector<std::shared_ptr<ICourse>> courses(const QString &languageId) const = 0;

    /**
     * @brief Requests a refresh of all resources
     *
     * Typical reasons to call this are GHNS signals
     */
    virtual void reloadCourses() = 0;

    /**
     * \return list of all available languages
     */
    virtual QVector<std::shared_ptr<ILanguage>> languages() const = 0;

Q_SIGNALS:
    void courseAboutToBeAdded(std::shared_ptr<ICourse>, int);
    void courseAdded();
    void courseAboutToBeRemoved(int);
    void courseRemoved();
};
Q_DECLARE_INTERFACE(IResourceRepository, "IResourceRepository")

#endif
