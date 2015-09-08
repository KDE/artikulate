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

#include "mainwindow_editor.h"
#include "ui/resourcesdialogpage.h"
#include "ui/sounddevicedialogpage.h"
#include "ui/appearencedialogpage.h"
#include "core/resourcemanager.h"
#include "core/editorsession.h"
#include "core/resources/courseresource.h"
#include "models/languagemodel.h"
#include "settings.h"
#include "libsound/src/outputdevicecontroller.h"

#include <KActionCollection>
#include <KConfigDialog>
#include <KDeclarative/KDeclarative>
#include <KLocalizedString>
#include <KMessageBox>
#include <KNS3/DownloadDialog>
#include <KStandardAction>

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QDebug>
#include <QDebug>
#include <QGraphicsObject>
#include <QIcon>
#include <QPointer>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickItem>
#include <QQuickView>
#include <QQuickWidget>
#include <QStandardPaths>

using namespace LearnerProfile;

MainWindowEditor::MainWindowEditor()
    : m_editorSession(new EditorSession(this))
    , m_resourceManager(new ResourceManager(this))
    , m_widget(new QQuickWidget)
{
    setWindowIcon(QIcon::fromTheme("artikulate"));
    setWindowTitle(qAppName());
    setAutoSaveSettings();

    // load saved sound settings
    OutputDeviceController::self().setVolume(Settings::audioOutputVolume());

    // load resources
    m_resourceManager->loadLanguageResources();
    if (m_resourceManager->languageResources().count() == 0) {
        qFatal("No language resources found, cannot start application.");
    }
    m_resourceManager->loadCourseResources();

    KDeclarative::KDeclarative kdeclarative;
    kdeclarative.setDeclarativeEngine(m_widget->engine());
    kdeclarative.setupBindings(); //TODO use result for determining touch/desktop version

    // create menu
    setupActions();

    // set view
    m_widget->resize(QSize(800, 600));
    m_widget->rootContext()->setContextProperty("g_resourceManager", m_resourceManager);
    m_widget->rootContext()->setContextProperty("editorSession", m_editorSession);

    // set starting screen
    m_widget->setSource(QUrl::fromLocalFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "artikulate/qml/Editor.qml")));
    m_widget->setResizeMode(QQuickWidget::SizeRootObjectToView);


    QAction *newAct = KStandardAction::save(this, SLOT(save()), actionCollection());
    actionCollection()->addAction("save", newAct);

    createGUI("artikulateui_editor.rc");
    setCentralWidget(m_widget);
}

MainWindowEditor::~MainWindowEditor()
{
    // save current settings for case of closing
    Settings::self()->save();
}

ResourceManager * MainWindowEditor::resourceManager() const
{
    return m_resourceManager;
}

void MainWindowEditor::setupActions()
{
    QAction *settingsAction = new QAction(i18nc("@item:inmenu", "Configure Artikulate"), this);
    connect(settingsAction, SIGNAL(triggered()), SLOT(showSettingsDialog()));
    actionCollection()->addAction("settings", settingsAction);
    settingsAction->setIcon(QIcon::fromTheme("configure"));

    KStandardAction::quit(qApp, SLOT(quit()), actionCollection());

    setupGUI(Keys | Save | Create, "artikulateui_editor.rc");
}

void MainWindowEditor::showSettingsDialog()
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

    connect(dialog, &QDialog::accepted, resourceDialog, &ResourcesDialogPage::saveSettings);
    connect(dialog, &QDialog::accepted, soundDialog, &SoundDeviceDialogPage::saveSettings);
    connect(dialog, &QDialog::accepted, appearenceDialog, &AppearenceDialogPage::saveSettings);

    dialog->exec();
}

void MainWindowEditor::save()
{
    m_resourceManager->sync();
}

bool MainWindowEditor::queryClose()
{
    if (!m_editorSession->course() || m_editorSession->course()->modified() == false) {
        return true;
    }

    int result = KMessageBox::warningYesNoCancel(0, i18nc("@info",
        "The currently open course contains unsaved changes. Do you want to save them?"));

    switch(result) {
    case KMessageBox::Yes:
        m_editorSession->course()->sync();
        return true;
    case KMessageBox::No:
        return true;
    default:
        return false;
    }
}
