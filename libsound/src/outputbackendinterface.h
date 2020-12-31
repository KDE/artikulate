/*
    SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef OUTPUTBACKENDINTERFACE_H
#define OUTPUTBACKENDINTERFACE_H

#include "libsound_export.h"
#include "outputdevicecontroller.h"
#include <QObject>

class LIBSOUND_EXPORT OutputBackendInterface : public QObject
{
    Q_OBJECT

public:
    explicit OutputBackendInterface(QObject *parent = nullptr);
    virtual ~OutputBackendInterface();
    virtual void setUri(const QString &uri) = 0;
    /**
     * volume as cubic value
     */
    virtual int volume() const = 0;
    virtual OutputDeviceController::State state() const = 0;

public Q_SLOTS:
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void setVolume(int volume) = 0;

Q_SIGNALS:
    void positionChanged();
    void stateChanged();
};

#endif
