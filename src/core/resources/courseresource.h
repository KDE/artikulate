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
#include "core/icourse.h"

#include <QObject>
#include <QVector>

class QDomElement;
class QString;
class CourseResourcePrivate;
class Course;
class Unit;
class Phrase;
class IResourceRepository;

//TODO move to private
class QXmlSchema;
class QJSonDocument;
class QDomDocument;

class ARTIKULATECORE_EXPORT CourseResource : public ICourse
{
    Q_OBJECT
    Q_INTERFACES(ICourse)

public:
    /**
     * Create course resource from file.
     */
    explicit CourseResource(const QUrl &path, IResourceRepository *repository);

    /**
     * @brief convenience constructor for porting to language access by dedicated model
     */
    explicit Q_DECL_DEPRECATED CourseResource(const QUrl &path, const QVector<Language *> &languages, IResourceRepository *repository);

    ~CourseResource() override;

    /**
     * \return unique identifier
     */
    QString id() const override;

    /**
     * \return human readable localized title
     */
    QString title() const override;

    /**
     * \return human readable title in English
     */
    QString i18nTitle() const override;

    /**
     * \return description text for course
     */
    QString description() const override;

    /**
     * \return language identifier of this course
     */
    Language * language() const override;

    /**
     * \return true if resource is loaded, otherwise false
     */
    bool isOpen() const;

    void sync();

    /**
     * export course as <course-id>.tar.bz2 file in the specified folder.
     */
    void exportGhns(const QString &path);

    /**
     * close resource without writing changes back to file
     */
    void close();

    QUrl file() const override;

    QList<Unit *> unitList() override;

    /**
     * \return reference to the loaded course resource
     */
    Q_DECL_DEPRECATED Course * course();

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
