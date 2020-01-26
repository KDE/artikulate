/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "application.h"

#include "core/contributorrepository.h"
#include "core/drawertrainingactions.h"
#include "core/editorsession.h"
#include "core/ieditablerepository.h"
#include "core/ilanguage.h"
#include "core/iresourcerepository.h"
#include "core/isessionactions.h"
#include "core/language.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"
#include "core/phrase.h"
#include "core/player.h"
#include "core/recorder.h"
#include "core/resources/editablecourseresource.h"
#include "core/resources/skeletonresource.h"
#include "core/trainingaction.h"
#include "core/trainingsession.h"
#include "core/unit.h"
#include "models/coursefiltermodel.h"
#include "models/coursemodel.h"
#include "models/languagemodel.h"
#include "models/languageresourcemodel.h"
#include "models/learningprogressmodel.h"
#include "models/phonemegroupmodel.h"
#include "models/phonememodel.h"
#include "models/phonemeunitmodel.h"
#include "models/phrasefiltermodel.h"
#include "models/phraselistmodel.h"
#include "models/phrasemodel.h"
#include "models/profilemodel.h"
#include "models/skeletonmodel.h"
#include "models/unitfiltermodel.h"
#include "models/unitmodel.h"
#include "qmlcontrols/iconitem.h"

#include "liblearnerprofile/src/learner.h"
#include "liblearnerprofile/src/learninggoal.h"
#include "liblearnerprofile/src/models/learninggoalmodel.h"
#include "liblearnerprofile/src/profilemanager.h"

#include <QObject>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
{
    registerQmlTypes();
}

IResourceRepository *Application::resourceRepository() const
{
    return m_resourceRepository;
}

IEditableRepository *Application::editableRepository() const
{
    return qobject_cast<IEditableRepository *>(m_resourceRepository);
}

void Application::installResourceRepository(IResourceRepository *resourceRepository)
{
    m_resourceRepository = resourceRepository;
}

void Application::registerQmlTypes()
{
    qmlRegisterUncreatableType<TrainingSession>("artikulate", 1, 0, "TrainingSession", QStringLiteral("TrainingSession is unique object provided by the backend"));
    qmlRegisterUncreatableType<EditorSession>("artikulate", 1, 0, "EditorSession", QStringLiteral("EditorSession is unique object provided by the backend"));
    qmlRegisterUncreatableType<ContributorRepository>("artikulate", 1, 0, "ContributorRepository", QStringLiteral("ContributorRepository is unique object provided by the backend"));
    qmlRegisterUncreatableType<LearnerProfile::ProfileManager>("artikulate", 1, 0, "ProfileManager", QStringLiteral("ProfileManager is unique object provided by the backend"));
    qmlRegisterUncreatableType<EditableCourseResource>("artikulate", 1, 0, "EditableCourseResource", QStringLiteral("EditableCourseResource objects are backend objects"));
    qmlRegisterUncreatableType<SkeletonResource>("artikulate", 1, 0, "SkeletonResource", QStringLiteral("SkeletonResource objects are backend objects"));
    qmlRegisterUncreatableType<Phrase>("artikulate", 1, 0, "Phrase", QStringLiteral("Phrase objects are backend objects"));
    qmlRegisterUncreatableType<Unit>("artikulate", 1, 0, "Unit", QStringLiteral("Unit objects are backend objects"));

    // interfaces
    qmlRegisterInterface<ICourse>("ICourse");
    qmlRegisterInterface<IEditableCourse>("IEditableCourse");
    qmlRegisterInterface<IEditablePhrase>("IEditablePhrase");
    qmlRegisterInterface<ILanguage>("ILanguage");
    qmlRegisterInterface<IPhrase>("IPhrase");
    qmlRegisterInterface<IResourceRepository>("IEditableRepository");
    qmlRegisterInterface<IResourceRepository>("IResourceRepository");
    qmlRegisterInterface<ISessionActions>("ISessionActions");
    qmlRegisterInterface<IUnit>("IUnit");

    // concrete instantiable types
    qmlRegisterType<DrawerTrainingActions>("artikulate", 1, 0, "DrawerTrainingActions");
    qmlRegisterType<IconItem>("artikulate", 1, 0, "Icon");
    qmlRegisterType<Language>("artikulate", 1, 0, "Language");
    qmlRegisterType<LearnerProfile::Learner>("artikulate", 1, 0, "Learner");
    qmlRegisterType<LearnerProfile::LearningGoal>("artikulate", 1, 0, "LearningGoal");
    qmlRegisterType<Phoneme>("artikulate", 1, 0, "Phoneme");
    qmlRegisterType<PhonemeGroup>("artikulate", 1, 0, "PhonemeGroup");
    qmlRegisterType<Player>("artikulate", 1, 0, "Player");
    qmlRegisterType<Recorder>("artikulate", 1, 0, "Recorder");
    qmlRegisterType<TrainingAction>("artikulate", 1, 0, "TrainingAction");

    // models
    qmlRegisterType<CourseFilterModel>("artikulate", 1, 0, "CourseFilterModel");
    qmlRegisterType<CourseModel>("artikulate", 1, 0, "CourseModel");
    qmlRegisterType<LanguageModel>("artikulate", 1, 0, "LanguageModel");
    qmlRegisterType<LanguageResourceModel>("artikulate", 1, 0, "LanguageResourceModel");
    qmlRegisterType<LearnerProfile::LearningGoalModel>("artikulate", 1, 0, "LearningGoalModel");
    qmlRegisterType<PhonemeGroupModel>("artikulate", 1, 0, "PhonemeGroupModel");
    qmlRegisterType<PhonemeModel>("artikulate", 1, 0, "PhonemeModel");
    qmlRegisterType<PhonemeUnitModel>("artikulate", 1, 0, "PhonemeUnitModel");
    qmlRegisterType<PhraseFilterModel>("artikulate", 1, 0, "PhraseFilterModel");
    qmlRegisterType<PhraseListModel>("artikulate", 1, 0, "PhraseListModel");
    qmlRegisterType<PhraseModel>("artikulate", 1, 0, "PhraseModel");
    qmlRegisterType<ProfileModel>("artikulate", 1, 0, "ProfileModel");
    qmlRegisterType<SkeletonModel>("artikulate", 1, 0, "SkeletonModel");
    qmlRegisterType<UnitFilterModel>("artikulate", 1, 0, "UnitFilterModel");
    qmlRegisterType<UnitModel>("artikulate", 1, 0, "UnitModel");
    //     qmlRegisterType<LearningProgressModel>("artikulate", 1, 0, "LearningProgressModel");//TODO must be ported to new trainingsession
}
