/*
    SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "application.h"
#include "core/contributorrepository.h"
#include "core/ieditablerepository.h"
#include "core/ilanguage.h"
#include "core/iresourcerepository.h"
#include "core/language.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"
#include "core/phrase.h"
#include "core/resources/editablecourseresource.h"
#include "core/resources/skeletonresource.h"
#include "core/unit.h"
#include "liblearnerprofile/src/learner.h"
#include "liblearnerprofile/src/learninggoal.h"
#include "liblearnerprofile/src/models/learninggoalmodel.h"
#include "liblearnerprofile/src/profilemanager.h"
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
#include <QQmlEngine>
#include <QString>

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
    constexpr QLatin1String uri{"org.kde.artikulate"};
    qmlRegisterUncreatableType<ContributorRepository>(uri.data(),
                                                      1,
                                                      0,
                                                      "ContributorRepository",
                                                      QStringLiteral("ContributorRepository is unique object provided by the backend"));
    qmlRegisterUncreatableType<LearnerProfile::ProfileManager>(uri.data(),
                                                               1,
                                                               0,
                                                               "ProfileManager",
                                                               QStringLiteral("ProfileManager is unique object provided by the backend"));
    qmlRegisterUncreatableType<EditableCourseResource>(uri.data(),
                                                       1,
                                                       0,
                                                       "EditableCourseResource",
                                                       QStringLiteral("EditableCourseResource objects are backend objects"));
    qmlRegisterUncreatableType<SkeletonResource>(uri.data(), 1, 0, "SkeletonResource", QStringLiteral("SkeletonResource objects are backend objects"));
    qmlRegisterUncreatableType<Phrase>(uri.data(), 1, 0, "Phrase", QStringLiteral("Phrase objects are backend objects"));
    qmlRegisterUncreatableType<Unit>(uri.data(), 1, 0, "Unit", QStringLiteral("Unit objects are backend objects"));

    // interfaces
    qmlRegisterInterface<ICourse>("ICourse", 1);
    qmlRegisterInterface<IEditableCourse>("IEditableCourse", 1);
    qmlRegisterInterface<IEditablePhrase>("IEditablePhrase", 1);
    qmlRegisterInterface<ILanguage>("ILanguage", 1);
    qmlRegisterInterface<IPhrase>("IPhrase", 1);
    qmlRegisterInterface<IResourceRepository>("IEditableRepository", 1);
    qmlRegisterInterface<IResourceRepository>("IResourceRepository", 1);
    qmlRegisterInterface<IUnit>("IUnit", 1);

    // concrete instantiable types
    qmlRegisterType<Language>(uri.data(), 1, 0, "Language");
    qmlRegisterType<LearnerProfile::Learner>(uri.data(), 1, 0, "Learner");
    qmlRegisterType<LearnerProfile::LearningGoal>(uri.data(), 1, 0, "LearningGoal");
    qmlRegisterType<Phoneme>(uri.data(), 1, 0, "Phoneme");
    qmlRegisterType<PhonemeGroup>(uri.data(), 1, 0, "PhonemeGroup");

    // models
    qmlRegisterType<CourseFilterModel>(uri.data(), 1, 0, "CourseFilterModel");
    qmlRegisterType<CourseModel>(uri.data(), 1, 0, "CourseModel");
    qmlRegisterType<LanguageModel>(uri.data(), 1, 0, "LanguageModel");
    qmlRegisterType<LanguageResourceModel>(uri.data(), 1, 0, "LanguageResourceModel");
    qmlRegisterType<LearnerProfile::LearningGoalModel>(uri.data(), 1, 0, "LearningGoalModel");
    qmlRegisterType<PhonemeGroupModel>(uri.data(), 1, 0, "PhonemeGroupModel");
    qmlRegisterType<PhonemeModel>(uri.data(), 1, 0, "PhonemeModel");
    qmlRegisterType<PhonemeUnitModel>(uri.data(), 1, 0, "PhonemeUnitModel");
    qmlRegisterType<PhraseFilterModel>(uri.data(), 1, 0, "PhraseFilterModel");
    qmlRegisterType<PhraseListModel>(uri.data(), 1, 0, "PhraseListModel");
    qmlRegisterType<PhraseModel>(uri.data(), 1, 0, "PhraseModel");
    qmlRegisterType<ProfileModel>(uri.data(), 1, 0, "ProfileModel");
    qmlRegisterType<SkeletonModel>(uri.data(), 1, 0, "SkeletonModel");
    qmlRegisterType<UnitFilterModel>(uri.data(), 1, 0, "UnitFilterModel");
    qmlRegisterType<UnitModel>(uri.data(), 1, 0, "UnitModel");
    //     qmlRegisterType<LearningProgressModel>("artikulate", 1, 0, "LearningProgressModel");//TODO must be ported to new trainingsession
}
