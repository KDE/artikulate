/*
    SPDX-FileCopyrightText: 2013-2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "mainwindow.h"
#include "application.h"
#include "artikulate_debug.h"
#include "libsound/src/outputdevicecontroller.h"
#include "settings.h"
#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <QIcon>
#include <QQmlContext>
#include <QStandardPaths>

using namespace LearnerProfile;

MainWindow::MainWindow()
    : m_profileManager(this)
    , m_trainingSession(&m_profileManager, this)
{
    rootContext()->setContextObject(new KLocalizedContext(this));

    // load saved sound settings
    OutputDeviceController::self().setVolume(Settings::audioOutputVolume());

    // set view
    rootContext()->setContextProperty(QStringLiteral("g_trainingSession"), &m_trainingSession);
    rootContext()->setContextProperty(QStringLiteral("g_profileManager"), &m_profileManager);
    rootContext()->setContextProperty(QStringLiteral("g_artikulateAboutData"), QVariant::fromValue(KAboutData::applicationData()));

    // set starting screen
    load(QUrl(QStringLiteral("qrc:/artikulate/qml/Main.qml")));

    // create training profile if none exists:
    if (!m_profileManager.activeProfile()) {
        m_profileManager.addProfile(i18n("Unnamed Identity"));
    }

    // connect to QML signals;
    connect(rootObjects().constFirst(), SIGNAL(ghnsCourseDataStatusChanged()), this, SLOT(updateCourseResources()));
}

MainWindow::~MainWindow()
{
    // save current settings for case of closing
    Settings::self()->save();
    m_profileManager.sync();
}

void MainWindow::updateCourseResources()
{
    artikulateApp->resourceRepository()->reloadCourses();
}

void MainWindow::updateKcfgUseContributorResources()
{
    rootContext()->setContextProperty(QStringLiteral("kcfg_UseContributorResources"), Settings::useCourseRepository());
}

void MainWindow::configLearnerProfile()
{
    qCritical() << "Not implemented"; // FIXME
}

bool MainWindow::queryClose()
{
    Settings::self()->save();
    // FIXME make sure all learner data is written to database
    return true;
}
