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
#include "version.h"

#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>
#include "artikulate_debug.h"

int main(int argc, char **argv)
{
    KLocalizedString::setApplicationDomain("artikulate");

    KAboutData aboutData("artikulate",
                         ki18nc("@title Displayed program name", "Artikulate").toString(),
                         ARTIKULATE_VERSION_STRING,
                         ki18nc("@title KAboutData: short program description", "Artikulate Pronunciation Trainer").toString(),
                         KAboutLicense::GPL_V2,
                         ki18nc("@info:credit", "(c) 2013-2015 The Artikulate Developers").toString(),
                         ki18nc("@title Short program description", "Training your pronunciation in a foreign language.").toString()
                        );

    aboutData.addAuthor(ki18nc("@info:credit Developer name", "Andreas Cord-Landwehr").toString(),
                        ki18nc("@info:credit Role", "Original Author").toString(),
                        "cordlandwehr@kde.org");

    aboutData.addAuthor(ki18nc("@info:credit Developer name", "Samikshan Bairagya").toString(),
                        ki18nc("@info:credit Role", "Developer").toString(),
                        "samikshan@gmail.com");

    aboutData.addAuthor(ki18nc("@info:credit Developer name", "Oindrila Gupta").toString(),
                        ki18nc("@info:credit Role", "Developer and Course Data").toString());

    aboutData.addAuthor(ki18nc("@info:credit Developer name", "Magdalena Konkiewicz").toString(),
                        ki18nc("@info:credit Role", "Developer and Course Data").toString());

    // TODO: for >=KF5.15 use KCrash::initialize();
    KCrash::setCrashHandler(KCrash::defaultCrashHandler);

    Application app(argc, argv);
    KAboutData::setApplicationData(aboutData);
    new MainWindow();

    return app.exec();
}
