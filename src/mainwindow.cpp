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
#include "ui/sounddevicedialogpage.h"

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
    QAction *settingsAction = new QAction(i18nc("@item:inmenu", "Configure Artikulate"), this);
    connect(settingsAction, &QAction::triggered, this, &MainWindow::showSettingsDialog);
    actionCollection()->addAction(QStringLiteral("settings"), settingsAction);
    settingsAction->setIcon(QIcon::fromTheme(QStringLiteral("configure")));

    QAction *configLearnerProfileAction = new QAction(i18nc("@item:inmenu", "Learner Profile"), this);
    connect(configLearnerProfileAction, &QAction::triggered, this, &MainWindow::configLearnerProfile);
    actionCollection()->addAction(QStringLiteral("config_learner_profile"), configLearnerProfileAction);
    configLearnerProfileAction->setIcon(QIcon::fromTheme(QStringLiteral("user-identity")));

    KStandardAction::quit(qApp, SLOT(quit()), actionCollection());
}

void MainWindow::showSettingsDialog()
{
    if (KConfigDialog::showDialog(QStringLiteral("settings"))) {
        return;
    }
    QPointer<KConfigDialog> dialog = new KConfigDialog(nullptr, QStringLiteral("settings"), Settings::self());

    SoundDeviceDialogPage *soundDialog = new SoundDeviceDialogPage();

    soundDialog->loadSettings();

    dialog->addPage(soundDialog, i18nc("@item:inmenu", "Sound Devices"), QStringLiteral("audio-headset"), i18nc("@title:tab", "Sound Device Settings"), true);

    connect(dialog.data(), &QDialog::accepted, soundDialog, &SoundDeviceDialogPage::saveSettings);
    connect(dialog.data(), &QDialog::accepted, this, &MainWindow::updateTrainingPhraseFont);
    connect(dialog.data(), &QDialog::accepted, this, &MainWindow::updateKcfgUseContributorResources);
    connect(dialog.data(), &QDialog::finished, soundDialog, &SoundDeviceDialogPage::stopPlaying);
    connect(dialog.data(), &QDialog::finished, soundDialog, &SoundDeviceDialogPage::stopRecord);

    dialog->exec();
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
