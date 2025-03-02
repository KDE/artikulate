/*
    SPDX-FileCopyrightText: 2019 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef ISESSIONACTIONS_H
#define ISESSIONACTIONS_H

#include <QList>
#include <QObject>
#include <QQmlEngine>

class ICourse;
class IPhrase;

/**
 * \class ISessionActions
 *
 * Interface for both training and editor sessions that exposes simple iterator functionalities for a selected course.
 */
class ISessionActions : public QObject
{
    Q_OBJECT
    QML_INTERFACE

public:
    explicit ISessionActions(QObject *parent)
        : QObject(parent)
    {
    }
    ~ISessionActions() override = default;
    virtual void setActivePhrase(IPhrase *phrase) = 0;
    virtual IPhrase *activePhrase() const = 0;

Q_SIGNALS:
    void courseChanged();
    void phraseChanged();
};
Q_DECLARE_INTERFACE(ISessionActions, "ISessionActions")

#endif
