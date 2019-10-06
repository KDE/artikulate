/*
 *  Copyright 2013-2017  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "application.h"
#include "mainwindow.h"
#include "version.h"

#include "artikulate_debug.h"
#include <KAboutData>
#include <KCrash>
#include <KLocalizedString>

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
                         i18nc("@info:credit", "(c) 2013-2019 The Artikulate Developers"),
                         i18nc("@title Short program description", "Train your pronunciation in a foreign language."));
    aboutData.setProgramLogo(app.windowIcon());

    aboutData.addAuthor(i18nc("@info:credit Developer name", "Andreas Cord-Landwehr"), i18nc("@info:credit Role", "Original Author"), QStringLiteral("cordlandwehr@kde.org"));

    aboutData.addAuthor(i18nc("@info:credit Developer name", "Samikshan Bairagya"), i18nc("@info:credit Role", "Developer"), QStringLiteral("samikshan@gmail.com"));

    aboutData.addAuthor(i18nc("@info:credit Developer name", "Oindrila Gupta"), i18nc("@info:credit Role", "Developer and Course Data"), QStringLiteral("edu@kde.org"));

    aboutData.addAuthor(i18nc("@info:credit Developer name", "Magdalena Konkiewicz"), i18nc("@info:credit Role", "Developer and Course Data"), QStringLiteral("edu@kde.org"));

    aboutData.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    KAboutData::setApplicationData(aboutData);
    KCrash::initialize();
    new MainWindow();

    return app.exec();
}
