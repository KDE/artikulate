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

#include "test_course.h"
#include "src/core/icourse.h"
#include "src/core/course.h"
#include "src/core/resources/courseresource.h"
#include "src/core/language.h"
#include "src/core/unit.h"
#include <QTest>
#include <QSignalSpy>
/*
class CourseStub : public ICourse
{
public:
    CourseStub(Language *language, QVector<Unit *> units)
        : m_language(language)
        , m_units(units)
    {
    }
    QString id() const override
    {
        return "courseid";
    }
    QString title() const override
    {
        return "title";
    }
    QString i18nTitle() const override
    {
        return "i18n title";
    }
    QString description() const override
    {
        return "description of the course";
    }
    Language * language() const override
    {
        return m_language;
    }
    QList<Unit *> unitList() const override
    {
        return m_units.toList();
    }
    QUrl file() const override
    {
        return QUrl();
    }

private:
    Language *m_language{nullptr};
    QVector<Unit *> m_units;
};*/

void TestCourse::init()
{
    // TODO initialization of test case
}

void TestCourse::cleanup()
{
    // TODO cleanup after test run
}

void TestCourse::createCourseWithoutUnits()
{
//    CourseResource resource(nullptr);

    Course course();
//TODO
//    virtual ~ICourse() = default;
//    virtual QString id() const = 0;
//    virtual QString title() const = 0;
//    virtual QString i18nTitle() const = 0;
//    virtual QString description() const = 0;
//    virtual Language * language() const = 0;
//    virtual QList<Unit *> unitList() const = 0;
//    virtual QUrl file() const = 0;

//    Language language;
//    CourseStub course(&language, QVector<Unit *>());
//    LearnerProfile::ProfileManager manager;
//    TrainingSession session(&manager);
//    session.setCourse(&course);
//    QVERIFY(&course == session.course());
}

void TestCourse::createCourseWithoutPhrases()
{
//    Language language;
//    Unit unitA;
//    Unit unitB;
//    Phrase phraseA1;
//    Phrase phraseA2;
//    Phrase phraseB1;
//    Phrase phraseB2;
//    // note: phrases without soundfiles are skipped in session generation
//    phraseA1.setId("A1");
//    phraseA2.setId("A2");
//    phraseB1.setId("B1");
//    phraseB2.setId("B2");
//    phraseA1.setSound(QUrl::fromLocalFile("/tmp/a1.ogg"));
//    unitA.addPhrase(&phraseA1);
//    unitA.addPhrase(&phraseA2);
//    unitB.addPhrase(&phraseB1);
//    unitB.addPhrase(&phraseB2);

//    CourseStub course(&language, QVector<Unit *>({&unitA, &unitB}));
//    LearnerProfile::ProfileManager manager;
//    TrainingSession session(&manager);
//    session.setCourse(&course);

//    // test number of actions
//    auto actions = session.trainingActions();
//    QCOMPARE(actions.count(), 1);
//    QCOMPARE(actions.at(0)->actions().count(), 1);
}

QTEST_GUILESS_MAIN(TestCourse)
