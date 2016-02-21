/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "application.h"

#include "core/skeleton.h"
#include "core/course.h"
#include "core/language.h"
#include "core/resourcemanager.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"
#include "core/trainingsession.h"
#include "core/editorsession.h"
#include "core/player.h"
#include "core/recorder.h"
#include "models/coursemodel.h"
#include "models/coursefiltermodel.h"
#include "models/languagemodel.h"
#include "models/languageresourcemodel.h"
#include "models/learningprogressmodel.h"
#include "models/unitmodel.h"
#include "models/unitfiltermodel.h"
#include "models/phrasemodel.h"
#include "models/phraselistmodel.h"
#include "models/phrasefiltermodel.h"
#include "models/phonememodel.h"
#include "models/phonemegroupmodel.h"
#include "models/phonemeunitmodel.h"
#include "models/profilemodel.h"
#include "models/skeletonmodel.h"
#include "qmlcontrols/iconitem.h"

#include "liblearnerprofile/src/learner.h"
#include "liblearnerprofile/src/profilemanager.h"
#include "liblearnerprofile/src/learninggoal.h"
#include "liblearnerprofile/src/models/learninggoalmodel.h"

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>
#include <QGraphicsDropShadowEffect>

Application::Application(int& argc, char** argv)
    : QApplication(argc, argv)
{
    registerQmlTypes();
}

void Application::registerQmlTypes()
{
    qmlRegisterUncreatableType<TrainingSession>(
        "artikulate", 1, 0,
        "TrainingSession",
        "TrainingSession is unique object provided by the backend");
    qmlRegisterUncreatableType<EditorSession>(
        "artikulate", 1, 0,
        "EditorSession",
        "EditorSession is unique object provided by the backend");
    qmlRegisterUncreatableType<ResourceManager>(
        "artikulate", 1, 0,
        "ResourceManager",
        "ResourceManager is unique object provided by the backend");
    qmlRegisterUncreatableType<LearnerProfile::ProfileManager>(
        "artikulate", 1, 0,
        "ProfileManager",
        "ProfileManager is unique object provided by the backend");

    qmlRegisterType<LearnerProfile::Learner>("artikulate", 1, 0, "Learner");
    qmlRegisterType<LearnerProfile::LearningGoal>("artikulate", 1, 0, "LearningGoal");
    qmlRegisterType<Unit>("artikulate", 1, 0, "Unit");
    qmlRegisterType<Skeleton>("artikulate", 1, 0, "Skeleton");
    qmlRegisterType<Course>("artikulate", 1, 0, "Course");
    qmlRegisterType<Language>("artikulate", 1, 0, "Language");
    qmlRegisterType<ResourceManager>("artikulate", 1, 0, "ResourceManager");
    qmlRegisterType<Phrase>("artikulate", 1, 0, "Phrase");
    qmlRegisterType<Phoneme>("artikulate", 1, 0, "Phoneme");
    qmlRegisterType<PhonemeGroup>("artikulate", 1, 0, "PhonemeGroup");
    qmlRegisterType<Player>("artikulate", 1, 0, "Player");
    qmlRegisterType<Recorder>("artikulate", 1, 0, "Recorder");
    qmlRegisterType<IconItem>("artikulate", 1, 0, "Icon");

    qmlRegisterType<CourseModel>("artikulate", 1, 0, "CourseModel");
    qmlRegisterType<CourseFilterModel>("artikulate", 1, 0, "CourseFilterModel");
    qmlRegisterType<LanguageModel>("artikulate", 1, 0, "LanguageModel");
    qmlRegisterType<LanguageResourceModel>("artikulate", 1, 0, "LanguageResourceModel");
//     qmlRegisterType<LearningProgressModel>("artikulate", 1, 0, "LearningProgressModel");//TODO must be ported to new trainingsession
    qmlRegisterType<UnitModel>("artikulate", 1, 0, "UnitModel");
    qmlRegisterType<UnitFilterModel>("artikulate", 1, 0, "UnitFilterModel");
    qmlRegisterType<PhraseModel>("artikulate", 1, 0, "PhraseModel");
    qmlRegisterType<PhraseListModel>("artikulate", 1, 0, "PhraseListModel");
    qmlRegisterType<PhraseFilterModel>("artikulate", 1, 0, "PhraseFilterModel");
    qmlRegisterType<PhonemeModel>("artikulate", 1, 0, "PhonemeModel");
    qmlRegisterType<PhonemeGroupModel>("artikulate", 1, 0, "PhonemeGroupModel");
    qmlRegisterType<PhonemeUnitModel>("artikulate", 1, 0, "PhonemeUnitModel");
    qmlRegisterType<ProfileModel>("artikulate", 1, 0, "ProfileModel");
    qmlRegisterType<SkeletonModel>("artikulate", 1, 0, "SkeletonModel");
    qmlRegisterType<LearnerProfile::LearningGoalModel>("artikulate", 1, 0, "LearningGoalModel");
}
