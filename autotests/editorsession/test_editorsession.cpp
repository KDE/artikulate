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

#include "test_editorsession.h"
#include "editablerepositorystub.h"
#include "src/core/editorsession.h"
#include "src/core/icourse.h"
#include "src/core/ieditablecourse.h"
#include "src/core/ieditablerepository.h"
#include "src/core/language.h"
#include "src/core/unit.h"
#include <QTest>
#include <QSignalSpy>

class EditableCourseStub : public IEditableCourse
{
public:
    EditableCourseStub(Language *language, QVector<Unit *> units)
        : m_language(language)
        , m_units(units)
    {
    }
    ~EditableCourseStub() override;

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
    Language * language() const override
    {
        return m_language;
    }
    void setLanguage(Language *language) override
    {
        m_language = language;
        emit languageChanged();
    }
    QList<Unit *> unitList() override
    {
        return m_units.toList();
    }
    QUrl file() const override
    {
        return QUrl();
    }

private:
    QString m_id{ "courseid" };
    QString m_foreignId{ "foreigncourseid" };
    QString m_title{ "title" };
    QString m_i18nTitle{ "i18n title" };
    QString m_description{ "description of the course" };
    Language *m_language{nullptr};
    QVector<Unit *> m_units;
};

// define one virtual method out of line to pin CourseStub to this translation unit
EditableCourseStub::~EditableCourseStub() = default;


void TestEditorSession::init()
{
    // TODO initialization of test case
}

void TestEditorSession::cleanup()
{
    // TODO cleanup after test run
}

void TestEditorSession::createEditorSession()
{
    Language language;
    EditableCourseStub course(&language, QVector<Unit *>());
    EditableRepositoryStub repository{ {&course} };
    EditorSession session;
    session.setRepository(&repository);
    QVERIFY(session.course() != nullptr);
    QCOMPARE(session.course()->id(), course.id());
}

QTEST_GUILESS_MAIN(TestEditorSession)
