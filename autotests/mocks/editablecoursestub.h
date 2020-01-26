/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef EDITABLECOURSESTUB_H
#define EDITABLECOURSESTUB_H

#include "src/core/ieditablecourse.h"
#include "src/core/ilanguage.h"
#include "src/core/unit.h"

#include <QObject>

class EditableCourseStub : public IEditableCourse
{
public:
    EditableCourseStub(std::shared_ptr<ILanguage> language, QVector<std::shared_ptr<Unit>> units)
        : IEditableCourse()
        , m_language(language)
        , m_units(units)
    {
    }
    ~EditableCourseStub() override;

    static std::shared_ptr<EditableCourseStub> create(std::shared_ptr<ILanguage> language, QVector<std::shared_ptr<Unit>> units)
    {
        auto course = std::make_shared<EditableCourseStub>(language, units);
        course->setSelf(course);
        for (auto &unit : units) {
            unit->setCourse(course);
        }
        return course;
    }

    void setSelf(std::shared_ptr<ICourse> self) override
    {
        m_self = self;
    }
    std::shared_ptr<IEditableCourse> self() const override
    {
        return std::static_pointer_cast<IEditableCourse>(m_self.lock());
    }
    QString id() const override
    {
        return m_id;
    }
    void setId(QString id) override
    {
        m_id = id;
        emit idChanged();
    }
    QString foreignId() const override
    {
        return m_foreignId;
    }
    void setForeignId(QString id) override
    {
        m_foreignId = id;
    }
    QString title() const override
    {
        return m_title;
    }
    void setTitle(QString title) override
    {
        m_title = title;
        emit titleChanged();
    }
    QString i18nTitle() const override
    {
        return m_i18nTitle;
    }
    void setI18nTitle(QString title) override
    {
        m_i18nTitle = title;
    }
    QString description() const override
    {
        return m_description;
    }
    void setDescription(QString description) override
    {
        m_description = description;
        emit descriptionChanged();
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
    void setLanguage(std::shared_ptr<ILanguage> language) override
    {
        m_language = language;
        emit languageChanged();
    }
    QVector<std::shared_ptr<Unit>> units() override
    {
        return m_units;
    }
    std::shared_ptr<Unit> addUnit(std::shared_ptr<Unit> unit) override
    {
        m_units.append(std::move(unit));
        auto unitPtr = m_units.last();
        unitPtr->setCourse(self());
        return unitPtr;
    }
    bool createPhraseAfter(IPhrase *previousPhrase) override
    {
        Q_UNUSED(previousPhrase)
        // not implemented
        return false;
    }
    bool deletePhrase(IPhrase *phrase) override
    {
        Q_UNUSED(phrase)
        // not implemented
        return false;
    }
    QUrl file() const override
    {
        return QUrl();
    }
    bool sync() override
    {
        return false;
    }
    void updateFrom(std::shared_ptr<ICourse>) override
    {
        // not implemented
    }
    bool isModified() const override
    {
        return false;
    }
    bool exportToFile(const QUrl &) const override
    {
        // do nothing
        return false;
    }
    void triggerUnitChanged(std::shared_ptr<IEditableUnit> unit)
    {
        emit unitChanged(unit);
    }

private:
    std::weak_ptr<ICourse> m_self;
    QString m_id {"courseid"};
    QString m_foreignId {"foreigncourseid"};
    QString m_title {"title"};
    QString m_i18nTitle {"i18n title"};
    QString m_description {"description of the course"};
    std::shared_ptr<ILanguage> m_language;
    QVector<std::shared_ptr<Unit>> m_units;
};

#endif
