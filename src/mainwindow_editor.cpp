/*
    SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "mainwindow_editor.h"
#include "core/editorsession.h"
#include "libsound/src/outputdevicecontroller.h"
#include "settings.h"
#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QIcon>
#include <QPointer>
#include <QQmlContext>
#include <QStandardPaths>

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
