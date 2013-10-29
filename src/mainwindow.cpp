/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
#include "core/profile.h"
#include "models/languagemodel.h"
#include "settings.h"
#include "liblearnerprofile/src/profilemanager.h"

#include <KMainWindow>
#include <KAction>
#include <KActionCollection>
#include <KCmdLineArgs>
#include <KGlobal>
#include <KIcon>
#include <KLocale>
#include <KStandardDirs>
#include <kdeclarative.h>
#include <KConfigDialog>
#include <KDebug>
#include <KStandardAction>
#include <KApplication>
#include <KMessageBox>
#include <KNS3/DownloadDialog>

#include <QGraphicsObject>
#include <QDeclarativeItem>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeProperty>
#include <QtCore/QCoreApplication>
#include <QtCore/QPointer>

using namespace LearnerProfile;

MainWindow::MainWindow()
    : KXmlGuiWindow(0)
    , m_view(new QDeclarativeView(this))
    , m_trainingProfile(new Profile(this))
    , m_editorProfile(new Profile(this))
    , m_resourceManager(new ResourceManager(this))
    , m_trainingSession(new TrainingSession(this))
    , m_profileManager(new ProfileManager(this))
{
    setWindowIcon(KIcon("artikulate")); // FIXME not present yet
    setWindowTitle(qAppName());
    setCentralWidget(m_view);

    setAutoSaveSettings();

    // load resources
    m_resourceManager->loadLanguageResources();
    m_resourceManager->loadCourseResources();

    KDeclarative m_kdeclarative;
    m_kdeclarative.setDeclarativeEngine(m_view->engine());
    m_kdeclarative.initialize();
    m_kdeclarative.setupBindings(); //TODO use result for determining touch/desktop version

    // create menu
    setupActions();

    // set view
    setMinimumSize(QSize(1000, 700));

    m_view->setStyleSheet("background-color: transparent;");
    m_view->rootContext()->setContextObject(this);

    m_view->setResizeMode(QDeclarativeView::SizeRootObjectToView);

    m_view->rootContext()->setContextProperty("userProfile", m_trainingProfile);
    m_view->rootContext()->setContextProperty("editorProfile", m_editorProfile);
    m_view->rootContext()->setContextProperty("trainingSession", m_trainingSession);
    m_view->rootContext()->setContextProperty("profileManager", m_profileManager);

    m_view->rootContext()->setContextProperty("kcfg_UseContributorResources", Settings::useCourseRepository());

    // set starting screen
    m_view->setSource(QUrl::fromLocalFile(KGlobal::dirs()->findResource("appdata", "qml/Main.qml")));

    // settings from kcfg values
    updateTrainingPhraseFont();

    // set initial view
    m_view->rootObject()->setProperty("viewMode", Trainer);

    // set font for the phrase in trainer to default from kcfg file
    QObject *phraseText = m_view->rootObject()->findChild<QObject*>("phraseText");
    phraseText->setProperty("font", Settings::trainingPhraseFont());
}

MainWindow::~MainWindow()
{
    // write application config
    Settings::self()->writeConfig();
}

ResourceManager * MainWindow::resourceManager() const
{
    return m_resourceManager;
}

void MainWindow::setupActions()
{
    KAction *editorAction = new KAction(i18nc("@option:check", "Course Editor mode"), this);
    connect(editorAction, SIGNAL(triggered()), SLOT(switchMode()));
    connect(this, SIGNAL(modeChanged(bool)), editorAction, SLOT(setChecked(bool)));
    actionCollection()->addAction("editor", editorAction);
    editorAction->setIcon(KIcon("artikulate-course-editor"));
    editorAction->setCheckable(true);
    editorAction->setChecked(false);

    KAction *settingsAction = new KAction(i18nc("@item:inmenu", "Configure Artikulate"), this);
    connect(settingsAction, SIGNAL(triggered()), SLOT(showSettingsDialog()));
    actionCollection()->addAction("settings", settingsAction);
    settingsAction->setIcon(KIcon("configure"));

    KAction *downloadsAction = new KAction(i18nc("@item:inmenu", "Download New Language Course"), this);
    connect(downloadsAction, SIGNAL(triggered(bool)), this, SLOT(downloadNewStuff()));
    actionCollection()->addAction("download_new_stuff", downloadsAction);
    downloadsAction->setIcon(KIcon("get-hot-new-stuff"));

    KAction *configLearnerProfileAction = new KAction(i18nc("@item:inmenu", "Learner Profile"), this);
    connect(configLearnerProfileAction, SIGNAL(triggered(bool)), this, SLOT(configLearnerProfile()));
    actionCollection()->addAction("config_learner_profile", configLearnerProfileAction);
    configLearnerProfileAction->setIcon(KIcon("user-identity"));

    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

    setupGUI(Default, "artikulateui.rc");
}

void MainWindow::showCourseEditor()
{
    m_view->rootObject()->setProperty("viewMode", Editor);
    // untoggle editor view mode
    emit modeChanged(true);
}

void MainWindow::closeCourseEditor()
{
    m_view->rootObject()->setProperty("viewMode", Trainer);
    // toggle editor view mode
    emit modeChanged(false);
}

void MainWindow::switchMode()
{
    if (m_view->rootObject()->property("viewMode") == Trainer){
         showCourseEditor();
    }
    else {
        closeCourseEditor();
    }
}

void MainWindow::showSettingsDialog()
{
    if (KConfigDialog::showDialog("settings")) {
        return;
    }
    QPointer<KConfigDialog> dialog = new KConfigDialog(this, "settings", Settings::self());

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

    dialog->exec();
}

void MainWindow::updateTrainingPhraseFont()
{
    QObject *phraseText = m_view->rootObject()->findChild<QObject*>("phraseText");
    QFont f = phraseText->property("font").value<QFont>();
    phraseText->setProperty("font", Settings::trainingPhraseFont());
}

void MainWindow::updateKcfgUseContributorResources()
{
    m_view->rootContext()->setContextProperty("kcfg_UseContributorResources", Settings::useCourseRepository());
}

void MainWindow::downloadNewStuff()
{
    QPointer<KNS3::DownloadDialog> dialog(new KNS3::DownloadDialog("artikulate.knsrc", this));
    if ( dialog->exec() == QDialog::Accepted ) {
        // do nothing
    }

    delete dialog;

    //update available courses
    m_resourceManager->loadCourseResources();
}

void MainWindow::configLearnerProfile()
{
    kError() << "Not implemented"; //FIXME
}

bool MainWindow::queryClose()
{
    if (!m_editorProfile->course() || m_editorProfile->course()->modified() == false) {
        return true;
    }

    int result = KMessageBox::warningYesNoCancel(this, i18nc("@info",
        "The currently open course contains unsaved changes. Do you want to save them?"));

    switch(result) {
    case KMessageBox::Yes:
        m_editorProfile->course()->sync();
        return true;
    case KMessageBox::No:
        return true;
    default:
        return false;
    }
}
