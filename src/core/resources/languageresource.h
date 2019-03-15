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

#ifndef LANGUAGERESOURCE_H
#define LANGUAGERESOURCE_H

#include "artikulatecore_export.h"
#include "resourceinterface.h"

#include <QObject>

class LanguageResourcePrivate;
class Language;

class ARTIKULATECORE_EXPORT LanguageResource : public ResourceInterface
{
    Q_OBJECT

public:
    explicit LanguageResource(ResourceManager *resourceManager, const QUrl &path);

    virtual ~LanguageResource();

    /**
     * \return unique identifier
     */
    QString identifier() override;

    /**
     * \return human readable localized title
     */
    QString title() override;

    /**
     * \return human readable title in English
     */
    QString i18nTitle() override;

    /**
     * \return type of resource
     */
    Type type() const override;

    /**
     * \return true if resource is loaded, otherwise false
     */
    bool isOpen() const override;

    /**
     * close resource without writing changes back to file
     */
    void close() override;

    /**
     * \return path to resource file
     */
    QUrl path() const override;

    /**
     * \return reference to the loaded resource
     * if resource is not open yet, it will be loaded
     */
    QObject * resource() override;

    /**
     * \return reference to the loaded language resource
     * Same behavior as \see resource() but casted to Language
     */
    Language * language();

private:
    const QScopedPointer<LanguageResourcePrivate> d;
};


#endif
