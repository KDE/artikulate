/*
    SPDX-FileCopyrightText: 2015 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "application.h"
#include "core/contributorrepository.h"
#include "mainwindow_editor.h"
#include "version.h"

#include "artikulate_debug.h"
#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>

int main(int argc, char **argv)
{
    Application app(argc, argv);
    KLocalizedString::setApplicationDomain("artikulate");
    ContributorRepository repository;
    repository.reloadCourses();
    app.installResourceRepository(&repository);

    KAboutData aboutData(QStringLiteral("artikulate_editor"),
                         ki18nc("@title Displayed program name", "Artikulate Editor").toString(),
                         ARTIKULATE_VERSION_STRING,
                         ki18nc("@title KAboutData: short program description", "Artikulate Course Editor").toString(),
                         KAboutLicense::GPL_V2,
                         ki18nc("@info:credit", "(c) 2013-2019 The Artikulate Developers").toString(),
                         ki18nc("@title Short program description", "Edit Artikulate course files.").toString());

    aboutData.addAuthor(ki18nc("@info:credit Developer name", "Andreas Cord-Landwehr").toString(), ki18nc("@info:credit Role", "Original Author").toString(), QStringLiteral("cordlandwehr@kde.org"));

    KAboutData::setApplicationData(aboutData);
    KCrash::initialize();
    MainWindowEditor mainWindow(&repository);

    return app.exec();
}
