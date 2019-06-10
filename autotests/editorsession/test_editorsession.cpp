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
#include "src/core/resources/skeletonresource.h"
#include "src/core/unit.h"
#include <QTest>
#include <QSignalSpy>

class EditableCourseStub : public IEditableCourse
{
public:
    EditableCourseStub(std::shared_ptr<Language> language, QVector<std::shared_ptr<Unit>> units)
        : IEditableCourse()
        , m_language(language)
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
    std::shared_ptr<Language> language() const override
    {
        return m_language;
    }
    void setLanguage(std::shared_ptr<Language> language) override
    {
        m_language = language;
        emit languageChanged();
    }
    QList<Unit *> unitList() override
    {
        QList<Unit *> rawList;
        for (auto unit : m_units) {
            rawList.append(unit.get());
        }
        return rawList;
    }
    std::shared_ptr<Unit> addUnit(std::unique_ptr<Unit> unit) override
    {
        m_units.append(std::move(unit));
        return m_units.last();
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
    std::shared_ptr<Language> m_language;
    QVector<std::shared_ptr<Unit>> m_units;
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
    std::shared_ptr<Language> languageGerman(new Language);
    languageGerman->setId("de");
    std::shared_ptr<Language> languageEnglish(new Language);
    languageEnglish->setId("en");

    std::shared_ptr<IEditableCourse> course(new EditableCourseStub(languageGerman, QVector<std::shared_ptr<Unit>>()));
    course->setLanguage(languageGerman);
    std::shared_ptr<IEditableCourse> skeleton(new SkeletonResource(QUrl(), nullptr));

    EditableRepositoryStub repository{
        {languageGerman, languageEnglish}, // languages
        {skeleton}, // skeletons
        {course} // courses
    };
    EditorSession session;
    session.setRepository(&repository);
    QVERIFY(session.course() == nullptr);
    QVERIFY(session.language() == nullptr);
    QVERIFY(session.skeleton() == nullptr);
}

void TestEditorSession::nonSkeletonSwitchingBehavior()
{
    std::shared_ptr<Language> languageGerman;
    languageGerman->setId("de");
    std::shared_ptr<Language> languageEnglish;
    languageEnglish->setId("en");
    std::shared_ptr<IEditableCourse> courseGerman(new EditableCourseStub(languageGerman, QVector<std::shared_ptr<Unit>>()));
    courseGerman->setId("course-german");
    std::shared_ptr<IEditableCourse> courseEnglish(new EditableCourseStub(languageEnglish, QVector<std::shared_ptr<Unit>>()));
    courseEnglish->setId("course-english");

    EditableRepositoryStub repository{
        {languageGerman, languageEnglish}, // languages
        {}, // skeletons
        {courseGerman, courseEnglish} // courses
    };
    EditorSession session;
    session.setRepository(&repository);

    QVERIFY(session.course() == nullptr);
    session.setCourse(courseGerman.get());
    QCOMPARE(session.course()->id(), courseGerman->id());
    QVERIFY(session.language() != nullptr);
    QCOMPARE(session.language()->id(), languageGerman->id());

    QVERIFY(session.language() != nullptr);
    QCOMPARE(session.language()->id(), languageGerman->id());
    session.setCourse(courseEnglish.get());
    QVERIFY(session.course() != nullptr);
    QCOMPARE(session.course()->id(), courseEnglish->id());
    QVERIFY(session.language() != nullptr);
    QCOMPARE(session.language()->id(), languageEnglish->id());

}

void TestEditorSession::skeletonSwitchingBehavior()
{
    std::shared_ptr<Language> languageGerman;
    languageGerman->setId("de");
    std::shared_ptr<Language> languageEnglish;
    languageEnglish->setId("en");
    std::shared_ptr<IEditableCourse> courseGermanA(new EditableCourseStub(languageGerman, QVector<std::shared_ptr<Unit>>()));
    courseGermanA->setId("course-german");
    courseGermanA->setForeignId("testskeletonA");
    std::shared_ptr<IEditableCourse> courseGermanB(new EditableCourseStub(languageGerman, QVector<std::shared_ptr<Unit>>()));
    courseGermanB->setId("course-german");
    courseGermanB->setForeignId("testskeletonB");
    std::shared_ptr<IEditableCourse> courseEnglishA(new EditableCourseStub(languageEnglish, QVector<std::shared_ptr<Unit>>()));
    courseEnglishA->setId("course-english");
    courseEnglishA->setForeignId("testskeletonA");
    std::shared_ptr<IEditableCourse> skeletonA(new SkeletonResource(QUrl(), nullptr));
    skeletonA->setId("testskeletonA");
    std::shared_ptr<IEditableCourse> skeletonB(new SkeletonResource(QUrl(), nullptr));
    skeletonB->setId("testskeletonB");

    EditableRepositoryStub repository{
        {languageGerman, languageEnglish}, // languages
        {skeletonA, skeletonB}, // skeletons
        {courseGermanA, courseEnglishA, courseGermanB} // courses
    };
    EditorSession session;
    session.setRepository(&repository);

    session.setSkeleton(skeletonA.get());
    Q_ASSERT(session.skeleton() != nullptr);
    QCOMPARE(session.skeleton()->id(), skeletonA->id());
    Q_ASSERT(session.course() != nullptr);
    QCOMPARE(session.course()->id(), courseGermanA->id());
    session.setCourse(courseEnglishA.get());
    Q_ASSERT(session.course() != nullptr);
    QCOMPARE(session.course()->id(), courseEnglishA->id());

    session.setCourse(courseGermanB.get());
    QVERIFY(session.skeleton() != nullptr);
    QCOMPARE(session.skeleton()->id(), skeletonB->id());
    QVERIFY(session.course() != nullptr);
    QCOMPARE(session.course()->id(), courseGermanB->id());
    QVERIFY(session.language() != nullptr);
    QCOMPARE(session.language()->id(), languageGerman->id());
}

QTEST_GUILESS_MAIN(TestEditorSession)
