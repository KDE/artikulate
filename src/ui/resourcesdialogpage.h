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

#ifndef RESOURCESDIALOGPAGE_H
#define RESOURCESDIALOGPAGE_H

#include "ui_resourcesdialogpage.h"
#include <QWidget>

class ResourceManager;
class Course;

class ResourcesDialogPage :
    public QWidget
{
    Q_OBJECT

public:
    explicit ResourcesDialogPage(ResourceManager *resourceMgr);
    virtual ~ResourcesDialogPage();

public slots:
    void saveSettings();
    void loadSettings();
    void updateHints();

private:
    Ui::ResourcesDialogPage *ui;
    ResourceManager *m_resourceManager;
    bool m_restartNeeded;
};

#endif