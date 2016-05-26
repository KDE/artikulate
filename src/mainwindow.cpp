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

#include "mainwindow.h"
#include "ui/resourcesdialogpage.h"
#include "ui/sounddevicedialogpage.h"
#include "ui/appearencedialogpage.h"
#include "core/resourcemanager.h"
#include "core/trainingsession.h"
#include "core/editorsession.h"
#include "core/resources/courseresource.h"
#include "models/languagemodel.h"
#include "settings.h"
#include "liblearnerprofile/src/profilemanager.h"
#include "liblearnerprofile/src/learner.h"
#include "libsound/src/outputdevicecontroller.h"
#include "artikulate_debug.h"

#include <KActionCollection>
#include <KConfigDialog>
#include <KHelpMenu>
#include <KLocalizedString>
#include <KLocalizedContext>
#include <KNS3/DownloadDialog>
#include <KStandardAction>

#include <QAction>
#include <QIcon>
#include <QPointer>
#include <QQmlContext>
#include <QStandardPaths>

using namespace LearnerProfile;

MainWindow::MainWindow()
    : m_actionCollection(new KActionCollection(this, "artikulate"))
    , m_helpMenu(new KHelpMenu)
    , m_resourceManager(new ResourceManager(this))
    , m_profileManager(new LearnerProfile::ProfileManager(this))
    , m_trainingSession(new TrainingSession(this))
{
    rootContext()->setContextObject(new KLocalizedContext(this));

    // load saved sound settings
    OutputDeviceController::self().setVolume(Settings::audioOutputVolume());

    // load resources
    m_resourceManager->loadLanguageResources();
    if (m_resourceManager->languageResources().count() == 0) {
        qFatal("No language resources found, cannot start application.");
    }
    m_resourceManager->loadCourseResources();
    m_trainingSession->setProfileManager(m_profileManager);

    // create menu
    setupActions();

    // set view
    rootContext()->setContextProperty("g_resourceManager", m_resourceManager);
    rootContext()->setContextProperty("g_trainingSession", m_trainingSession);
    rootContext()->setContextProperty("g_profileManager", m_profileManager);
    rootContext()->setContextProperty("kcfg_UseContributorResources", Settings::useCourseRepository());
    rootContext()->setContextProperty("kcfg_ShowMenuBar", Settings::showMenuBar());

    // set starting screen
    load(QUrl("qrc:/artikulate/qml/Main.qml"));

    // settings from kcfg values
//     updateTrainingPhraseFont(); //FIXME deactivated while porting

    // create training profile if none exists:
    if (!m_profileManager->activeProfile()) {
        m_profileManager->addProfile(i18n("Unnamed Identity"));
    }

    // connect to QML signals
    connect(rootObjects().first(), SIGNAL(triggerDownloadCourses()),
            this, SLOT(downloadNewStuff()));
    connect(rootObjects().first(), SIGNAL(triggerSettingsDialog()),
            this, SLOT(showSettingsDialog()));
    connect(rootObjects().first(), SIGNAL(triggerAction(QString)),
            this, SLOT(triggerAction(QString)));
    connect(rootObjects().first(), SIGNAL(switchMenuBarVisibility()),
            this, SLOT(switchMenuBarVisibility()));

    // set font for the phrase in trainer to default from kcfg file
    QObject *phraseText = rootObjects().first()->findChild<QObject*>("phraseText");
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

ResourceManager * MainWindow::resourceManager() const
{
    return m_resourceManager;
}

KActionCollection * MainWindow::actionCollection()
{
    return m_actionCollection;
}

void MainWindow::setupActions()
{
    QAction *settingsAction = new QAction(i18nc("@item:inmenu", "Configure Artikulate"), this);
    connect(settingsAction, SIGNAL(triggered()), SLOT(showSettingsDialog()));
    actionCollection()->addAction("settings", settingsAction);
    settingsAction->setIcon(QIcon::fromTheme("configure"));

    QAction *downloadsAction = new QAction(i18nc("@item:inmenu", "Download New Language Course"), this);
    connect(downloadsAction, SIGNAL(triggered(bool)), this, SLOT(downloadNewStuff()));
    actionCollection()->addAction("download_new_stuff", downloadsAction);
    downloadsAction->setIcon(QIcon::fromTheme("get-hot-new-stuff"));

    QAction *configLearnerProfileAction = new QAction(i18nc("@item:inmenu", "Learner Profile"), this);
    connect(configLearnerProfileAction, SIGNAL(triggered(bool)), this, SLOT(configLearnerProfile()));
    actionCollection()->addAction("config_learner_profile", configLearnerProfileAction);
    configLearnerProfileAction->setIcon(QIcon::fromTheme("user-identity"));

    KStandardAction::helpContents(m_helpMenu, SLOT(appHelpActivated()), actionCollection());
    KStandardAction::reportBug(m_helpMenu, SLOT(reportBug()), actionCollection());
    KStandardAction::aboutKDE(m_helpMenu, SLOT(aboutKDE()), actionCollection());
    KStandardAction::aboutApp(m_helpMenu, SLOT(aboutApplication()), actionCollection());

    KStandardAction::quit(qApp, SLOT(quit()), actionCollection());
}

void MainWindow::showSettingsDialog()
{
    if (KConfigDialog::showDialog("settings")) {
        return;
    }
    QPointer<KConfigDialog> dialog = new KConfigDialog(0, "settings", Settings::self());

    ResourcesDialogPage *resourceDialog = new ResourcesDialogPage(m_resourceManager);
    SoundDeviceDialogPage *soundDialog = new SoundDeviceDialogPage();
    AppearenceDialogPage *appearenceDialog = new AppearenceDialogPage();

    resourceDialog->loadSettings();
    soundDialog->loadSettings();
    appearenceDialog->loadSettings();

    dialog->addPage(soundDialog, i18nc("@item:inmenu", "Sound Devices"), "audio-headset", i18nc("@title:tab", "Sound Device Settings"), true);
    dialog->addPage(appearenceDialog, i18nc("@item:inmenu", "Fonts"), "preferences-desktop-font", i18nc("@title:tab", "Training Phrase Font"), true);
    dialog->addPage(resourceDialog, i18nc("@item:inmenu", "Course Resources"), "repository", i18nc("@title:tab", "Resource Repository Settings"), true);

//     connect(dialog, SIGNAL(settingsChanged(const QString&)), resourceDialog, SLOT(loadSettings()));
//     connect(dialog, SIGNAL(settingsChanged(const QString&)), soundDialog, SLOT(loadSettings()));
    connect(dialog, SIGNAL(accepted()), resourceDialog, SLOT(saveSettings()));
    connect(dialog, SIGNAL(accepted()), soundDialog, SLOT(saveSettings()));
    connect(dialog, SIGNAL(accepted()), appearenceDialog, SLOT(saveSettings()));
    connect(dialog, SIGNAL(accepted()), SLOT(updateTrainingPhraseFont()));
    connect(dialog, SIGNAL(accepted()), SLOT(updateKcfgUseContributorResources()));
    connect(dialog, SIGNAL(finished()), soundDialog, SLOT(stopPlaying()));
    connect(dialog, SIGNAL(finished()), soundDialog, SLOT(stopRecord()));

    dialog->exec();
}

void MainWindow::updateTrainingPhraseFont()
{
    QObject *phraseText = rootObjects().first()->findChild<QObject*>("phraseText");
    if (!phraseText) {
        qCDebug(ARTIKULATE_LOG) << "no phraseText context object found, aborting";
        return;
    }
    QFont f = phraseText->property("font").value<QFont>();
    phraseText->setProperty("font", Settings::trainingPhraseFont());
}

void MainWindow::updateKcfgUseContributorResources()
{
    rootContext()->setContextProperty("kcfg_UseContributorResources", Settings::useCourseRepository());
}

void MainWindow::downloadNewStuff()
{
    QPointer<KNS3::DownloadDialog> dialog = new KNS3::DownloadDialog("artikulate.knsrc");
    if (dialog->exec() == QDialog::Accepted) {
        //update available courses
        m_resourceManager->loadCourseResources();

        if (!m_profileManager->activeProfile()) {
            qCWarning(ARTIKULATE_LOG) << "Not registering course language for favorite languages:"
                << "no active learner profile set";
            delete dialog;
            return;
        }

        // add languages of new courses to favorite languages
        foreach (const KNS3::Entry &entry, dialog->changedEntries()) {
            foreach (const QString &path, entry.installedFiles()) {
                if (!path.endsWith(QLatin1String(".xml"))) {
                    continue;
                }
                CourseResource *resource = new CourseResource(m_resourceManager, QUrl::fromLocalFile(path));
                // set as active course if no training course is set
                if (!m_trainingSession->course()) {
                    m_trainingSession->setCourse(resource->course());
                    m_trainingSession->setLanguage(resource->course()->language());
                }
                // update learning goals
                //TODO will be gone after refactoring
                foreach (LearningGoal *goal, m_profileManager->goals()) {
                    if (goal->category() == LearningGoal::Language
                        && goal->identifier() == resource->language()
                    ) {
                        // Learner::addGoal() checks for uniqueness of added goals
                        m_profileManager->activeProfile()->addGoal(goal);
                        break;
                    }
                }
                resource->deleteLater();
            }
        }
    }
    delete dialog;
}

void MainWindow::configLearnerProfile()
{
    qCritical() << "Not implemented"; //FIXME
}

void MainWindow::triggerAction(const QString &actionName)
{
    QAction * action = actionCollection()->action(actionName);
    if (action) {
        action->trigger();
    } else {
        qCritical() << "Action is not registered:" << actionName;
    }
}

void MainWindow::switchMenuBarVisibility()
{
    Settings::setShowMenuBar(!Settings::showMenuBar());
    rootContext()->setContextProperty("kcfg_ShowMenuBar", Settings::showMenuBar());
}

bool MainWindow::queryClose()
{
    Settings::self()->save();
    // FIXME make sure all learner data is written to database
    return true;
}
