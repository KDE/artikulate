/*
    SPDX-FileCopyrightText: 2013-2017 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "application.h"
#include "artikulate_debug.h"
#include "core/resourcerepository.h"
#include "mainwindow.h"
#include "version.h"
#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>
#include <QCommandLineParser>
#include <QIcon>

int main(int argc, char **argv)
{
    Application app(argc, argv);
    KLocalizedString::setApplicationDomain("artikulate");
    ResourceRepository repository;
    repository.reloadCourses();
    app.installResourceRepository(&repository);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("artikulate")));

    KAboutData aboutData(QStringLiteral("artikulate"),
                         i18nc("@title Displayed program name", "Artikulate"),
                         ARTIKULATE_VERSION_STRING,
                         i18nc("@title KAboutData: short program description", "Artikulate Pronunciation Trainer"),
                         KAboutLicense::GPL_V2,
                         i18nc("@info:credit", "(c) 2013-2022 The Artikulate Developers"),
                         i18nc("@title Short program description", "Train your pronunciation in a foreign language."));
    aboutData.setProgramLogo(app.windowIcon());

    aboutData.addAuthor(i18nc("@info:credit Developer name", "Andreas Cord-Landwehr"),
                        i18nc("@info:credit Role", "Original Author"),
                        QStringLiteral("cordlandwehr@kde.org"));

    aboutData.addAuthor(i18nc("@info:credit Developer name", "Samikshan Bairagya"),
                        i18nc("@info:credit Role", "Developer"),
                        QStringLiteral("samikshan@gmail.com"));

    aboutData.addAuthor(i18nc("@info:credit Developer name", "Oindrila Gupta"),
                        i18nc("@info:credit Role", "Developer and Course Data"),
                        QStringLiteral("edu@kde.org"));

    aboutData.addAuthor(i18nc("@info:credit Developer name", "Magdalena Konkiewicz"),
                        i18nc("@info:credit Role", "Developer and Course Data"),
                        QStringLiteral("edu@kde.org"));

    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    KAboutData::setApplicationData(aboutData);
    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);
    KCrash::initialize();
    MainWindow window;

    return app.exec();
}
