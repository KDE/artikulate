/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef TRAININGACTIONICON_H
#define TRAININGACTIONICON_H

#include <QColor>
#include <QObject>
#include <QQmlEngine>

class TrainingActionIcon : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER m_name CONSTANT)
    Q_PROPERTY(QString source MEMBER m_source CONSTANT)
    Q_PROPERTY(int width MEMBER m_width)
    Q_PROPERTY(int height MEMBER m_height)
    Q_PROPERTY(QColor color MEMBER m_color CONSTANT)

    QML_ELEMENT
    QML_UNCREATABLE("typed backend element")

public:
    explicit TrainingActionIcon(QObject *parent,
                                const QString &name = QString(),
                                const QString &source = QString(),
                                int width = 60,
                                int height = 60,
                                const QColor &color = QColor(0, 0, 0));

private:
    QString m_name{QString()};
    QString m_source{QString()};
    int m_width{60};
    int m_height{60};
    QColor m_color;
};

#endif
