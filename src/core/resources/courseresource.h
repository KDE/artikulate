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

class QDomElement;
class QString;
class CourseResourcePrivate;
class Course;
class Unit;
class Phrase;

class ARTIKULATECORE_EXPORT CourseResource : public ResourceInterface
{
    Q_OBJECT
    void course(QString text);

public:
    /**
     * Create course resource from file.
     */
    explicit CourseResource(ResourceManager *resourceManager, const QUrl &path);

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
     * export course as <course-id>.tar.bz2 file in the specified folder.
     */
    void exportGhns(const QString &path);

    /**
     * close resource without writing changes back to file
     */
    virtual void close();

    /**
     * \return path to resource file
     */
    virtual QUrl path() const;

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
    Phrase * parsePhrase(QDomElement phraseNode, Unit *parentUnit) const;
    /**
     * \return serialized course as DOM document
     * \param trainingExport if true phrases without recording and empty units are excluded
     */
    QDomDocument serializedDocument(bool trainingExport=false) const;
    QDomElement serializedPhrase(Phrase * phrase, QDomDocument &document) const;

    const QScopedPointer<CourseResourcePrivate> d;
};

#endif
