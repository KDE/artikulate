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
#include <memory>

class QString;
class CourseResourcePrivate;
class Unit;
class Phrase;
class ILanguage;
class IResourceRepository;
class EditableCourseResource;

class ARTIKULATECORE_EXPORT CourseResource : public ICourse
{
    Q_OBJECT
    Q_INTERFACES(ICourse)

public:
    static std::shared_ptr<CourseResource> create(const QUrl &path, IResourceRepository *repository);

    ~CourseResource() override;

    /**
     * \return unique identifier
     */
    QString id() const override;

    void setId(const QString &id);

    /**
     * \return global ID for this course
     */
    QString foreignId() const override;

    void setForeignId(const QString &foreignId);

    /**
     * \return human readable localized title
     */
    QString title() const override;

    void setTitle(const QString &title);

    /**
     * \return human readable title in English
     */
    QString i18nTitle() const override;

    void setI18nTitle(const QString &i18nTitle);

    /**
     * \return description text for course
     */
    QString description() const override;

    void setDescription(const QString &description);

    /**
     * \return language identifier of this course
     */
    std::shared_ptr<ILanguage> language() const override;

    QString languageTitle() const override;

    void setLanguage(std::shared_ptr<ILanguage> language);

    std::shared_ptr<Unit> addUnit(std::shared_ptr<Unit> unit);

    void sync();

    QUrl file() const override;

    QVector<std::shared_ptr<Unit>> units() override;

Q_SIGNALS:
    void idChanged();
    void foreignIdChanged();
    void titleChanged();
    void i18nTitleChanged();
    void descriptionChanged();
    void languageChanged();

private:
    /**
     * Create course resource from file.
     */
    explicit CourseResource(const QUrl &path, IResourceRepository *repository);
    void setSelf(std::shared_ptr<ICourse> self) override;
    std::shared_ptr<ICourse> self() const;
    const std::unique_ptr<CourseResourcePrivate> d;

    friend EditableCourseResource;
};

#endif
