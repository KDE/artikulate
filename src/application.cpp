/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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

#include "mainwindow.h"
#include "core/profile.h"
#include "core/course.h"
#include "core/language.h"
#include "core/resourcemanager.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "core/phoneme.h"
#include "core/phonemegroup.h"
#include "core/trainingsession.h"
#include "declarativeitems/applicationbackground.h"
#include "models/coursemodel.h"
#include "models/languagemodel.h"
#include "models/languageresourcemodel.h"
#include "models/unitmodel.h"
#include "models/phrasemodel.h"
#include "models/phrasefiltermodel.h"
#include "models/phonememodel.h"
#include "models/phonemegroupmodel.h"
#include "models/phonemeunitmodel.h"
#include "models/skeletonmodel.h"

#include <kdeclarative.h>

#include <qdeclarative.h>
#include <QGraphicsDropShadowEffect>
#include <QScriptValue>
#include <QScriptEngine>


Application::Application()
    : KApplication(true)
{
    registerQmlTypes();
}

void Application::registerQmlTypes()
{
    qmlRegisterType<Profile>("artikulate", 1, 0, "Profile");
    qmlRegisterType<Unit>("artikulate", 1, 0, "Unit");
    qmlRegisterType<Course>("artikulate", 1, 0, "Course");
    qmlRegisterType<Language>("artikulate", 1, 0, "Language");
    qmlRegisterType<ResourceManager>("artikulate", 1, 0, "ResourceManager");
    qmlRegisterType<Phrase>("artikulate", 1, 0, "Phrase");
    qmlRegisterType<Phoneme>("artikulate", 1, 0, "Phoneme");
    qmlRegisterType<PhonemeGroup>("artikulate", 1, 0, "PhonemeGroup");
    qmlRegisterType<TrainingSession>("artikulate", 1, 0, "TrainingSession");

    qmlRegisterType<CourseModel>("artikulate", 1, 0, "CourseModel");
    qmlRegisterType<LanguageModel>("artikulate", 1, 0, "LanguageModel");
    qmlRegisterType<LanguageResourceModel>("artikulate", 1, 0, "LanguageResourceModel");
    qmlRegisterType<UnitModel>("artikulate", 1, 0, "UnitModel");
    qmlRegisterType<PhraseModel>("artikulate", 1, 0, "PhraseModel");
    qmlRegisterType<PhraseFilterModel>("artikulate", 1, 0, "PhraseFilterModel");
    qmlRegisterType<PhonemeModel>("artikulate", 1, 0, "PhonemeModel");
    qmlRegisterType<PhonemeGroupModel>("artikulate", 1, 0, "PhonemeGroupModel");
    qmlRegisterType<PhonemeUnitModel>("artikulate", 1, 0, "PhonemeUnitModel");
    qmlRegisterType<SkeletonModel>("artikulate", 1, 0, "SkeletonModel");

    qmlRegisterType<MainWindow>("artikulate", 1, 0, "MainWindow");
    qmlRegisterType<ApplicationBackground>("artikulate", 1, 0, "ApplicationBackground");
}
