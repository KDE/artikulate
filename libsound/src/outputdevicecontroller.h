/*
    SPDX-FileCopyrightText: 2013 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef OUTPUTDEVICECONTROLLER_H
#define OUTPUTDEVICECONTROLLER_H

#include "libsound_export.h"
#include <QObject>

class OutputDeviceControllerPrivate;
class QUrl;

/**
 * \class OutputDeviceController
 *
 * This singleton class provides a controller for the sound output device.
 */
class LIBSOUND_EXPORT OutputDeviceController : public QObject
{
    Q_OBJECT

public:
    enum State { StoppedState, PlayingState, PausedState };

    /**
     * Returns self reference to the controller. First call of this method initializes
     * output device controller.
     *
     * \return self reference
     */
    static OutputDeviceController &self();

    void play(const QString &filePath);
    void play(const QUrl &filePath);
    OutputDeviceController::State state() const;
    void stop();
    QString currentSource() const;
    void setVolume(int volume);
    int volume() const;

public Q_SLOTS:
    void emitChangedState();

Q_SIGNALS:
    void started();
    void stopped();

private:
    Q_DISABLE_COPY(OutputDeviceController)
    /**
     * \internal
     * Private constructor, \ref self().
     */
    OutputDeviceController();

    /**
     * Private destructor.
     */
    ~OutputDeviceController();

    const QScopedPointer<OutputDeviceControllerPrivate> d;
};

#endif
