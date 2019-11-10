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
#include "application.h"
#include "artikulate_debug.h"
#include "core/editorsession.h"
#include "core/resources/courseresource.h"
#include "libsound/src/outputdevicecontroller.h"
#include "models/languagemodel.h"
#include "settings.h"
#include "ui/exportghnsdialog.h"
#include "ui/sounddevicedialogpage.h"
#include <KAboutData>
#include <KActionCollection>
#include <KConfigDialog>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KStandardAction>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QIcon>
#include <QPointer>
#include <QQmlContext>
#include <QQmlProperty>
#include <QQuickItem>
#include <QQuickView>
#include <QStandardPaths>
#include <QStatusBar>

using namespace LearnerProfile;

MainWindowEditor::MainWindowEditor(ContributorRepository *repository)
    : m_repository(repository)
    , m_editorSession(new EditorSession())
{
    rootContext()->setContextObject(new KLocalizedContext(this));
    rootContext()->setContextProperty(QStringLiteral("g_repository"), m_repository);
    rootContext()->setContextProperty(QStringLiteral("g_editorSession"), m_editorSession);
    rootContext()->setContextProperty(QStringLiteral("g_artikulateAboutData"), QVariant::fromValue(KAboutData::applicationData()));

    m_repository->setStorageLocation(QUrl::fromLocalFile(Settings::courseRepositoryPath()));
    m_editorSession->setRepository(m_repository);

    // load saved sound settings
    OutputDeviceController::self().setVolume(Settings::audioOutputVolume());

    m_repository->reloadCourses();

    // create menu
    setupActions();

    // set starting screen
    load(QUrl(QStringLiteral("qrc:/artikulate/qml/Editor.qml")));
}

MainWindowEditor::~MainWindowEditor()
{
    // save current settings for case of closing
    Settings::self()->save();
}

ContributorRepository *MainWindowEditor::resourceRepository() const
{
    return m_repository;
}

void MainWindowEditor::setupActions()
{
    //    QAction *settingsAction = new QAction(i18nc("@item:inmenu", "Configure Artikulate"), this);
    //    connect(settingsAction, &QAction::triggered, this, &MainWindowEditor::showSettingsDialog);
    //    actionCollection()->addAction(QStringLiteral("settings"), settingsAction);
    //    settingsAction->setIcon(QIcon::fromTheme(QStringLiteral("configure")));

    //    QAction *exportAction = new QAction(i18nc("@item:inmenu", "Export GHNS Files"), this);
    //    connect(exportAction, &QAction::triggered, this, [=]() {
    //        QPointer<QDialog> dialog = new ExportGhnsDialog(m_repository);
    //        dialog->exec();
    //    });
    //    actionCollection()->addAction(QStringLiteral("export_ghns"), exportAction);
    //    exportAction->setIcon(QIcon::fromTheme(QStringLiteral("document-export")));

    //    KStandardAction::quit(this, SLOT(quit()), actionCollection());
}

void MainWindowEditor::showSettingsDialog()
{
    if (KConfigDialog::showDialog(QStringLiteral("settings"))) {
        return;
    }
    QPointer<KConfigDialog> dialog = new KConfigDialog(nullptr, QStringLiteral("settings"), Settings::self());

    SoundDeviceDialogPage *soundDialog = new SoundDeviceDialogPage();

    soundDialog->loadSettings();

    dialog->addPage(soundDialog, i18nc("@item:inmenu", "Sound Devices"), QStringLiteral("audio-headset"), i18nc("@title:tab", "Sound Device Settings"), true);

    connect(dialog.data(), &QDialog::accepted, soundDialog, &SoundDeviceDialogPage::saveSettings);

    dialog->exec();
}

void MainWindowEditor::save()
{
    m_repository->sync();
}

void MainWindowEditor::quit()
{
    //    if (queryClose()) {
    //        qApp->quit();
    //    }
}

// bool MainWindowEditor::queryClose()
//{
//    if (!m_repository->modified()) {
//        return true;
//    }

//    int result = KMessageBox::warningYesNoCancel(nullptr, i18nc("@info",
//        "The currently open course contains unsaved changes. Do you want to save them?"));

//    switch(result) {
//    case KMessageBox::Yes:
//        m_repository->sync();
//        return true;
//    case KMessageBox::No:
//        return true;
//    default:
//        return false;
//    }
//}
