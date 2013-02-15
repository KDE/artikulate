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

#include "editorview.h"

#include <KDebug>

#include <QPropertyAnimation>
#include <QDeclarativeEngine>
#include <QDeclarativeComponent>
#include <QDeclarativeItem>
#include <QCoreApplication>
#include <QDeclarativeContext>
#include <QResizeEvent>

#include <KStandardDirs>
#include <kdeclarative.h>

EditorView::EditorView(QWidget *parent, const QString &file)
    : QDeclarativeView(parent)
{
    setResizeMode(SizeRootObjectToView);

    KDeclarative m_kdeclarative;
    m_kdeclarative.setDeclarativeEngine(engine());
    m_kdeclarative.initialize();

    // bind kconfig, icons, etc.
    m_kdeclarative.setupBindings();


    setSource(QUrl("qrc:/qml/Editor.qml"));
    kDebug() << errors();
//     Q_ASSERT(errors().isEmpty());

    connect(engine(), SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));

}

EditorView::~EditorView()
{}

void EditorView::resizeEvent(QResizeEvent *event)
{
    QDeclarativeView::resizeEvent(event);
}
