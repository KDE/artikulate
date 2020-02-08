/*
 *  Copyright 2013-2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include "mainwindow.h"
#include "application.h"
#include "artikulate_debug.h"
#include "core/editorsession.h"
#include "core/resourcerepository.h"
#include "core/resources/courseresource.h"
#include "core/trainingsession.h"
#include "liblearnerprofile/src/learner.h"
#include "liblearnerprofile/src/profilemanager.h"
#include "libsound/src/outputdevicecontroller.h"
#include "models/languagemodel.h"
#include "settings.h"

#include <KAboutData>
#include <KActionCollection>
#include <KConfigDialog>
#include <KHelpMenu>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KNS3/DownloadDialog>
#include <KStandardAction>

#include <QAction>
#include <QIcon>
#include <QPointer>
#include <QQmlContext>
#include <QStandardPaths>

using namespace LearnerProfile;

MainWindow::MainWindow()
    : m_actionCollection(new KActionCollection(this, QStringLiteral("artikulate")))
    , m_profileManager(new LearnerProfile::ProfileManager(this))
    , m_trainingSession(new TrainingSession(m_profileManager, this))
{
    rootContext()->setContextObject(new KLocalizedContext(this));

    // load saved sound settings
    OutputDeviceController::self().setVolume(Settings::audioOutputVolume());

    // create menu
    setupActions();

    // set view
    rootContext()->setContextProperty(QStringLiteral("g_trainingSession"), m_trainingSession);
    rootContext()->setContextProperty(QStringLiteral("g_profileManager"), m_profileManager);
    rootContext()->setContextProperty(QStringLiteral("g_artikulateAboutData"), QVariant::fromValue(KAboutData::applicationData()));

    // set starting screen
    load(QUrl(QStringLiteral("qrc:/artikulate/qml/Main.qml")));

    // create training profile if none exists:
    if (!m_profileManager->activeProfile()) {
        m_profileManager->addProfile(i18n("Unnamed Identity"));
    }

    // connect to QML signals;
    connect(rootObjects().constFirst(), SIGNAL(ghnsCourseDataStatusChanged()), this, SLOT(updateCourseResources()));
    connect(rootObjects().constFirst(), SIGNAL(triggerAction(QString)), this, SLOT(triggerAction(QString)));

    // set font for the phrase in trainer to default from kcfg file
    QObject *phraseText = rootObjects().constFirst()->findChild<QObject *>(QStringLiteral("phraseText"));
    if (phraseText) {
        phraseText->setProperty("font", Settings::trainingPhraseFont());
    }
}

MainWindow::~MainWindow()
{
    // save current settings for case of closing
    Settings::self()->save();
    m_profileManager->sync();
}

KActionCollection *MainWindow::actionCollection()
{
    return m_actionCollection;
}

void MainWindow::setupActions()
{
    QAction *configLearnerProfileAction = new QAction(i18nc("@item:inmenu", "Learner Profile"), this);
    connect(configLearnerProfileAction, &QAction::triggered, this, &MainWindow::configLearnerProfile);
    actionCollection()->addAction(QStringLiteral("config_learner_profile"), configLearnerProfileAction);
    configLearnerProfileAction->setIcon(QIcon::fromTheme(QStringLiteral("user-identity")));

    KStandardAction::quit(qApp, SLOT(quit()), actionCollection());
}

void MainWindow::updateCourseResources()
{
    artikulateApp->resourceRepository()->reloadCourses();
}

void MainWindow::updateTrainingPhraseFont()
{
    QObject *phraseText = rootObjects().constFirst()->findChild<QObject *>(QStringLiteral("phraseText"));
    if (!phraseText) {
        qCDebug(ARTIKULATE_LOG) << "no phraseText context object found, aborting";
        return;
    }
    phraseText->setProperty("font", Settings::trainingPhraseFont());
}

void MainWindow::updateKcfgUseContributorResources()
{
    rootContext()->setContextProperty(QStringLiteral("kcfg_UseContributorResources"), Settings::useCourseRepository());
}

void MainWindow::configLearnerProfile()
{
    qCritical() << "Not implemented"; // FIXME
}

void MainWindow::triggerAction(const QString &actionName)
{
    QAction *action = actionCollection()->action(actionName);
    if (action) {
        action->trigger();
    } else {
        qCritical() << "Action is not registered:" << actionName;
    }
}

void MainWindow::switchMenuBarVisibility()
{
    Settings::setShowMenuBar(!Settings::showMenuBar());
    rootContext()->setContextProperty(QStringLiteral("kcfg_ShowMenuBar"), Settings::showMenuBar());
}

bool MainWindow::queryClose()
{
    Settings::self()->save();
    // FIXME make sure all learner data is written to database
    return true;
}
