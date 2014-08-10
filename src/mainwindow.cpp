/*
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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
#include "core/resources/courseresource.h"
#include "models/languagemodel.h"
#include "settings.h"
#include "liblearnerprofile/src/profilemanager.h"
#include "liblearnerprofile/src/learner.h"
#include "libsound/src/outputdevicecontroller.h"

#include <KMainWindow>
#include <QAction>
#include <KActionCollection>
#include <KCmdLineArgs>
#include <KDialog>
#include <KGlobal>
#include <QIcon>
#include <KLocale>
#include <KDeclarative/KDeclarative>
#include <KConfigDialog>
#include <QDebug>
#include <KStandardAction>
#include <KApplication>
#include <KMessageBox>
#include <KNS3/DownloadDialog>

#include <QGraphicsObject>
#include <QQuickItem>
#include <QQuickView>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickWidget>
#include <QCloseEvent>
#include <QtCore/QCoreApplication>
#include <QtCore/QPointer>
#include <QStandardPaths>

using namespace LearnerProfile;

MainWindow::MainWindow()
    : KXmlGuiWindow(0)
    , m_widget(new QQuickWidget(this)) //FIXME set MainWindow*
    , m_trainingProfile(new Profile(this))
    , m_editorProfile(new Profile(this))
    , m_resourceManager(new ResourceManager(this))
    , m_trainingSession(new TrainingSession(this))
    , m_profileManager(new LearnerProfile::ProfileManager(this))
{
    setWindowIcon(QIcon::fromTheme("artikulate")); // FIXME not present yet
    setWindowTitle(qAppName());
    setAutoSaveSettings();

    // load saved sound settings
    OutputDeviceController::self().setVolume(Settings::audioOutputVolume());

    // load resources
    m_resourceManager->loadLanguageResources();
    m_resourceManager->loadCourseResources();
    m_resourceManager->registerLearningGoals(m_profileManager);

    KDeclarative::KDeclarative m_kdeclarative;
    m_kdeclarative.setDeclarativeEngine(m_widget->engine());
    m_kdeclarative.initialize();
    m_kdeclarative.setupBindings(); //TODO use result for determining touch/desktop version

    // create menu
    setupActions();

    // set view
    m_widget->resize(QSize(800, 600));
    m_widget->setStyleSheet("background-color: transparent;");
    m_widget->rootContext()->setContextObject(this);

    m_widget->rootContext()->setContextProperty("userProfile", m_trainingProfile); //TODO deprecated
    m_widget->rootContext()->setContextProperty("editorProfile", m_editorProfile); //TODO rename
    m_widget->rootContext()->setContextProperty("trainingSession", m_trainingSession); //TODO needed at top level?
    m_widget->rootContext()->setContextProperty("profileManager", m_profileManager);

    m_widget->rootContext()->setContextProperty("kcfg_UseContributorResources", Settings::useCourseRepository());

    m_widget->setStyleSheet("background-color: transparent;");

    // set starting screen
    m_widget->setSource(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::DataLocation, "qml/Main.qml")));

    // settings from kcfg values
    updateTrainingPhraseFont();

    // create training profile if none exists:
    if (!m_profileManager->activeProfile()) {
        m_profileManager->addProfile("Unnamed Identity"); //TODO use i18n for 4.14
    }

    // set initial view
    m_widget->rootObject()->setProperty("viewMode", Trainer);

    // set font for the phrase in trainer to default from kcfg file
    QObject *phraseText = m_widget->rootObject()->findChild<QObject*>("phraseText");
    phraseText->setProperty("font", Settings::trainingPhraseFont());

    setCentralWidget(m_widget);
}

MainWindow::~MainWindow()
{
    // save current settings for case of closing
    Settings::self()->writeConfig();
    m_profileManager->sync();
}

ResourceManager * MainWindow::resourceManager() const
{
    return m_resourceManager;
}

void MainWindow::setupActions()
{
    QAction *editorAction = new QAction(i18nc("@option:check", "Course Editor mode"), this);
    connect(editorAction, SIGNAL(triggered()), SLOT(switchMode()));
    connect(this, SIGNAL(modeChanged(bool)), editorAction, SLOT(setChecked(bool)));
    actionCollection()->addAction("editor", editorAction);
    editorAction->setIcon(QIcon::fromTheme("artikulate-course-editor"));
    editorAction->setCheckable(true);
    editorAction->setChecked(false);

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

    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

    setupGUI(Keys | Save | Create, "artikulateui.rc");
}

void MainWindow::showCourseEditor()
{
    m_widget->rootObject()->setProperty("viewMode", Editor);
    // untoggle editor view mode
    emit modeChanged(true);
}

void MainWindow::closeCourseEditor()
{
    m_widget->rootObject()->setProperty("viewMode", Trainer);
    // toggle editor view mode
    emit modeChanged(false);
}

void MainWindow::switchMode()
{
    if (m_widget->rootObject()->property("viewMode") == Trainer){
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
    dialog->setButtons(KDialog::Cancel | KDialog::Ok);

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
    QObject *phraseText = m_widget->rootObject()->findChild<QObject*>("phraseText");
    if (!phraseText) {
        qDebug() << "no phraseText context object found, aborting";
        return;
    }
    QFont f = phraseText->property("font").value<QFont>();
    phraseText->setProperty("font", Settings::trainingPhraseFont());
}

void MainWindow::updateKcfgUseContributorResources()
{
    m_widget->rootContext()->setContextProperty("kcfg_UseContributorResources", Settings::useCourseRepository());
}

void MainWindow::downloadNewStuff()
{
    QPointer<KNS3::DownloadDialog> dialog = new KNS3::DownloadDialog("artikulate.knsrc", this);
    if (dialog->exec() == QDialog::Accepted) {
        //update available courses
        m_resourceManager->loadCourseResources();

        if (!m_profileManager->activeProfile()) {
            qWarning() << "Not registering course language for favorite languages:"
                << " no active learner profile set";
            delete dialog;
            return;
        }

        // add languages of new courses to favorite languages
        foreach (const KNS3::Entry &entry, dialog->changedEntries()) {
            foreach (const QString &path, entry.installedFiles()) {
                if (!path.endsWith(QLatin1String(".xml"))) {
                    continue;
                }
                CourseResource *resource = new CourseResource(m_resourceManager, path);
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
