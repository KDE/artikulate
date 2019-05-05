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

#ifndef SKELETONRESOURCE_H
#define SKELETONRESOURCE_H

#include "artikulatecore_export.h"

#include <QObject>

class SkeletonResourcePrivate;
class Skeleton;

class ARTIKULATECORE_EXPORT SkeletonResource : public QObject
{
    Q_OBJECT
    void course(QString text);

public:
    /**
     * Create course resource from file.
     */
    explicit SkeletonResource(const QUrl &path);

    /**
     * Create course resource from course.
     */
    explicit SkeletonResource(Skeleton *skeleton);

    virtual ~SkeletonResource();

    /**
     * \return unique identifier
     */
    QString identifier();

    /**
     * \return human readable localized title
     */
    QString title();

    /**
     * \return human readable title in English
     */
    QString i18nTitle();

    /**
     * \return true if resource is loaded, otherwise false
     */
    bool isOpen() const;

    /**
     * close resource without writing changes back to file
     */
    void close();

    void sync();

    void reload();

    /**
     * \return path to resource file
     */
    QUrl path() const;

    /**
     * \return reference to the loaded resource
     * if resource is not open yet, it will be loaded
     */
    QObject * resource();

    /**
     * \return reference to the loaded skeleton resource
     * Same behavior as \see resource() but casted to Skeleton
     */
    Skeleton * skeleton();

private:
    const QScopedPointer<SkeletonResourcePrivate> d;
};

#endif
