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
#include "core/resourcemanager.h"
#include "core/profile.h"
#include "models/languagemodel.h"
#include "settings.h"

#include <KMainWindow>
#include <KAction>
#include <KActionCollection>
#include <KCmdLineArgs>
#include <KGlobal>
#include <KHelpMenu>
#include <KIcon>
#include <KLocale>
#include <KStandardDirs>
#include <kdeclarative.h>
#include <KMenu>
#include <KConfigDialog>
#include <KDebug>

#include <QGraphicsObject>
#include <QDeclarativeItem>
#include <QDeclarativeView>
#include <QDeclarativeContext>
#include <QDeclarativeProperty>
#include <QtCore/QCoreApplication>
#include <QtCore/QPointer>

MainWindow::MainWindow()
    : KMainWindow()
    , m_view(new QDeclarativeView(this))
    , m_actionCollection(new KActionCollection(this))
    , m_menu(new KMenu(this))
    , m_profile(new Profile(this))
    , m_resourceManager(new ResourceManager(this))
{
    setWindowIcon(KIcon("artikulate")); // FIXME not present yet
    setWindowTitle(qAppName());
    setCentralWidget(m_view);

    setAutoSaveSettings();

    // load resources
    m_resourceManager->loadResources();

    KDeclarative m_kdeclarative;
    m_kdeclarative.setDeclarativeEngine(m_view->engine());
    m_kdeclarative.initialize();

    // create menu
    m_actionCollection->addAssociatedWidget(this);
    m_menu->addSeparator();
    KAction *editorAction = new KAction(i18n("Course Editor"), this);
    connect(editorAction, SIGNAL(triggered()), SLOT(showCourseEditor()));
    m_actionCollection->addAction("editor", editorAction);
    m_menu->addAction(editorAction);

    KAction *settingsAction = new KAction(i18n("Settings"), this);
    connect(settingsAction, SIGNAL(triggered()), SLOT(showSettingsDialog()));
    m_actionCollection->addAction("settings", settingsAction);
    m_menu->addAction(settingsAction);

    m_menu->addSeparator();

    KHelpMenu *helpMenu = new KHelpMenu(m_menu, KCmdLineArgs::aboutData(), false, m_actionCollection);
    m_menu->addMenu(helpMenu->menu());

    // set view
    m_view->setMinimumSize(1000, 700);
    m_view->setStyleSheet("background-color: transparent;");
    m_view->rootContext()->setContextObject(this);

    m_view->setResizeMode(QDeclarativeView::SizeRootObjectToView);

    m_view->rootContext()->setContextProperty("viewMode", Trainer);
    m_view->rootContext()->setContextProperty("userProfile", m_profile);

    // set starting screen
    m_view->setSource(QUrl::fromLocalFile(KGlobal::dirs()->findResource("appdata", "qml/Main.qml")));

    // set initial view
    m_view->rootObject()->setProperty("viewMode", Trainer);
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

void MainWindow::showCourseEditor()
{
    m_view->rootObject()->setProperty("viewMode", Editor);
}

void MainWindow::closeCourseEditor()
{
    m_view->rootObject()->setProperty("viewMode", Trainer);
}

void MainWindow::showMenu(int xPos, int yPos)
{
    m_menu->popup(m_view->mapToGlobal(QPoint(xPos, yPos)));
}

void MainWindow::showSettingsDialog()
{
    if (KConfigDialog::showDialog("settings")) {
        return;
    }
    QPointer<KConfigDialog> dialog = new KConfigDialog(this, "settings", Settings::self());

    ResourcesDialogPage *resourceDialog = new ResourcesDialogPage(m_resourceManager);
    SoundDeviceDialogPage *soundDialog = new SoundDeviceDialogPage();
    resourceDialog->loadSettings();
    soundDialog->loadSettings();

    dialog->addPage(soundDialog, i18nc("@item:inmenu", "Sound Devices"), "audio-headset", i18nc("@title:tab", "Sound Device Settings"), true);
    dialog->addPage(resourceDialog, i18nc("@item:inmenu", "Course Resources"), "repository", i18nc("@title:tab", "Resource Repository Settings"), true);

//     connect(dialog, SIGNAL(settingsChanged(const QString&)), resourceDialog, SLOT(loadSettings()));
//     connect(dialog, SIGNAL(settingsChanged(const QString&)), soundDialog, SLOT(loadSettings()));
    connect(dialog, SIGNAL(accepted()), resourceDialog, SLOT(saveSettings()));
    connect(dialog, SIGNAL(accepted()), soundDialog, SLOT(saveSettings()));

    dialog->exec();
}
