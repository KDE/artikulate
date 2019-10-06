/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
