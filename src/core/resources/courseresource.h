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

#ifndef COURSERESOURCE_H
#define COURSERESOURCE_H

#include "artikulatecore_export.h"
#include "resourceinterface.h"

#include <QObject>

class CourseResourcePrivate;
class Course;

class ARTIKULATELIB_EXPORT CourseResource : public ResourceInterface
{
    Q_OBJECT
    void course(QString text);

public:
    /**
     * Create course resource from file.
     */
    explicit CourseResource(ResourceManager *resourceManager, const KUrl &path);

    virtual ~CourseResource();

    /**
     * \return unique identifier
     */
    virtual QString identifier();

    /**
     * \return human readable localized title
     */
    virtual QString title();

    /**
     * \return human readable title in English
     */
    virtual QString i18nTitle();

    /**
     * \return language identifier of this course
     */
    QString language() const;

    /**
     * \return type of resource
     */
    virtual Type type() const;

    /**
     * \return true if resource is loaded, otherwise false
     */
    virtual bool isOpen() const;

    virtual void sync();

    /**
     * close resource without writing changes back to file
     */
    virtual void close();

    /**
     * \return path to resource file
     */
    virtual KUrl path() const;

    /**
     * \return reference to the loaded resource
     * if resource is not open yet, it will be loaded
     */
    virtual QObject * resource();

    /**
     * \return reference to the loaded course resource
     * Same behavior as \see resource() but casted to Course
     */
    Course * course();

private:
    const QScopedPointer<CourseResourcePrivate> d;
};

#endif
