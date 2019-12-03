/*
 *  Copyright 2015  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef MAINWINDOW_EDITOR_H
#define MAINWINDOW_EDITOR_H

#include "core/contributorrepository.h"
#include "core/trainingsession.h"
#include <QQmlApplicationEngine>

class EditorSession;
class QQuickWidget;

class MainWindowEditor : public QQmlApplicationEngine
{
    Q_OBJECT

public:
    /**
     * Default Constructor
     */
    explicit MainWindowEditor(ContributorRepository *repository);

    /**
     * Default Destructor
     */
    virtual ~MainWindowEditor();

    ContributorRepository *resourceRepository() const;

    void setupActions();

public Q_SLOTS:
    void showSettingsDialog();
    void save();
    void quit();

Q_SIGNALS:
    void modeChanged(bool);

private:
    ContributorRepository *m_repository;
    EditorSession *m_editorSession;
};

#endif
