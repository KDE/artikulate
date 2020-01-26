/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef COURSESTUB_H
#define COURSESTUB_H

#include "src/core/icourse.h"
#include "src/core/language.h"
#include "src/core/unit.h"

#include <QObject>

class CourseStub : public ICourse
{
public:
    CourseStub(std::shared_ptr<ILanguage> language, QVector<std::shared_ptr<Unit>> units)
        : m_language(language)
        , m_units(units)
    {
    }
    ~CourseStub() override;

    static std::shared_ptr<ICourse> create(std::shared_ptr<ILanguage> language, QVector<std::shared_ptr<Unit>> units)
    {
        auto course = std::make_shared<CourseStub>(language, units);
        course->setSelf(course);
        return std::static_pointer_cast<ICourse>(course);
    }

    void setSelf(std::shared_ptr<ICourse> self) override
    {
        m_self = self;
    }

    QString id() const override
    {
        return "courseid";
    }
    QString foreignId() const override
    {
        return "foreigncourseid";
    }
    QString title() const override
    {
        return m_title;
    }
    void setTitle(QString title)
    {
        m_title = title;
        emit titleChanged();
    }
    QString i18nTitle() const override
    {
        return "i18n title";
    }
    QString description() const override
    {
        return "description of the course";
    }
    std::shared_ptr<ILanguage> language() const override
    {
        return m_language;
    }
    QString languageTitle() const override
    {
        if (m_language) {
            m_language->title();
        }
        return QString();
    }
    QVector<std::shared_ptr<Unit>> units() override
    {
        return m_units;
    }
    QUrl file() const override
    {
        return QUrl();
    }

private:
    QString m_title {"title"};
    std::weak_ptr<ICourse> m_self;
    std::shared_ptr<ILanguage> m_language;
    QVector<std::shared_ptr<Unit>> m_units;
};

#endif
