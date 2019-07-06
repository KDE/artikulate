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

#include "test_coursemodel.h"
#include "src/core/icourse.h"
#include <QTest>
#include <QSignalSpy>

// assumption: during a training session the units and phrases of a course do not change
//   any change of such a course shall result in a reload of a training session

class CourseStub : public ICourse
{
public:
    CourseStub(std::shared_ptr<Language> language, QVector<std::shared_ptr<Unit>> units)
        : m_language(language)
        , m_units(units)
    {
    }
    ~CourseStub() override;

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
    std::shared_ptr<Language> language() const override
    {
        return m_language;
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
    std::weak_ptr<ICourse> m_self;
    std::shared_ptr<Language> m_language;
    QVector<std::shared_ptr<Unit>> m_units;
};

// define one virtual method out of line to pin CourseStub to this translation unit
CourseStub::~CourseStub() = default;

void TestCourseModel::init()
{
    // TODO initialization of test case
}

void TestCourseModel::cleanup()
{
    // TODO cleanup after test run
}

QTEST_GUILESS_MAIN(TestCourseModel)
