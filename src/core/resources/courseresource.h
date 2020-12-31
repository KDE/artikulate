/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
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
    static std::shared_ptr<CourseResource> create(const QUrl &path, IResourceRepository *repository, bool skipIncomplete = false);

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
    explicit CourseResource(const QUrl &path, IResourceRepository *repository, bool skipIncomplete);
    void setSelf(std::shared_ptr<ICourse> self) override;
    std::shared_ptr<ICourse> self() const;
    const std::unique_ptr<CourseResourcePrivate> d;

    friend EditableCourseResource;
};

#endif
