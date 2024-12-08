/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "trainingactionicon.h"

TrainingActionIcon::TrainingActionIcon(QObject *parent, const QString &name, const QString &source, int width, int height, const QColor &color)
    : QObject(parent)
    , m_name(name)
    , m_source(source)
    , m_width(width)
    , m_height(height)
    , m_color(color)
{
}
