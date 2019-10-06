/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef TRAININGACTIONICON_H
#define TRAININGACTIONICON_H

#include "artikulatecore_export.h"
#include <QColor>
#include <QObject>

class ARTIKULATECORE_EXPORT TrainingActionIcon : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name CONSTANT)
    Q_PROPERTY(QString source MEMBER m_source CONSTANT)
    Q_PROPERTY(int width MEMBER m_width)
    Q_PROPERTY(int height MEMBER m_height)
    Q_PROPERTY(QColor color MEMBER m_color CONSTANT)

public:
    explicit TrainingActionIcon(QObject *parent, const QString &name = QString(), const QString &source = QString(), int width = 60, int height = 60, const QColor &color = QColor(0, 0, 0));

private:
    QString m_name {QString()};
    QString m_source {QString()};
    int m_width {60};
    int m_height {60};
    QColor m_color;
};

#endif
