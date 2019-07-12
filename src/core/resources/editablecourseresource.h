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

#ifndef EDITABLECOURSERESOURCE_H
#define EDITABLECOURSERESOURCE_H

#include "artikulatecore_export.h"
#include "courseresource.h"
#include "core/icourse.h"
#include "core/ieditablecourse.h"

#include <memory>
#include <QObject>
#include <QVector>

class IResourceRepository;
class Course;
class Unit;
class Phrase;
class QString;
class QDomDocument;

/**
 * @brief Decorator for CourseResource
 *
 * This decorator adds functionality to modify and write back changes of a course.
 */
class ARTIKULATECORE_EXPORT EditableCourseResource : public IEditableCourse
{
    Q_OBJECT
    Q_INTERFACES(ICourse)
    Q_INTERFACES(IEditableCourse)

public:
    static std::shared_ptr<EditableCourseResource> create(const QUrl &path, IResourceRepository *repository);
    ~EditableCourseResource() override = default;

    /**
     * \return unique identifier
     */
    QString id() const override;

    void setId(QString id) override;

    /**
     * \return unique identifier
     */
    QString foreignId() const override;

    void setForeignId(QString foreignId) override;

    /**
     * \return human readable localized title
     */
    QString title() const override;

    void setTitle(QString title) override;

    /**
     * \return human readable title in English
     */
    QString i18nTitle() const override;

    void setI18nTitle(QString i18nTitle) override;

    /**
     * \return description text for course
     */
    QString description() const override;

    void setDescription(QString description) override;
    /**
     * \return language identifier of this course
     */
    std::shared_ptr<ILanguage> language() const override;
    void setLanguage(std::shared_ptr<ILanguage> language) override;
    bool sync() override;
    bool exportToFile(const QUrl &filePath) const override;

    std::shared_ptr<Unit> addUnit(std::unique_ptr<Unit> unit) override;
    QVector<std::shared_ptr<Unit>> units() override;
    bool isModified() const override;
    QUrl file() const override;

    Q_INVOKABLE Unit * createUnit();
    Q_INVOKABLE Phrase * createPhrase(Unit *unit);

Q_SIGNALS:
    void idChanged();
    void foreignIdChanged();
    void titleChanged();
    void i18nTitleChanged();
    void descriptionChanged();
    void languageChanged();

private:
    Q_DISABLE_COPY(EditableCourseResource)
    /**
     * Create course resource from file.
     */
    explicit EditableCourseResource(const QUrl &path, IResourceRepository *repository);
    void setSelf(std::shared_ptr<ICourse> self) override;
    bool m_modified{ false };
    const std::unique_ptr<CourseResource> m_course;
};

#endif
