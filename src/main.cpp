/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@gmail.com>
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

#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KDebug>

static const char description[] =
    I18N_NOOP("Learn and practice pronunciation.");

static const char version[] = "0.0.1";

int main(int argc, char **argv)
{
    KAboutData about("artikulate", 0,
                ki18n("Artikulate Pronunciation Trainer"),
                version,
                ki18n(description),
                KAboutData::License_GPL,
                ki18n("Copyright (C) 2013 by Andreas Cord-Landwehr")
                );

    about.addAuthor(ki18n("Andreas Cord-Landwehr"), ki18n("Original Author"), "cordlandwehr@gmail.com");

    KCmdLineArgs::init(argc, argv, &about);
    KApplication::setGraphicsSystem("raster");

    Application app;

    MainWindow *widget = new MainWindow;
    widget->show();

    return app.exec();
}
