/*
 *  Copyright 2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef OUTPUTBACKENDINTERFACE_H
#define OUTPUTBACKENDINTERFACE_H

#include "outputdevicecontroller.h"
#include "libsound_export.h"
#include <QObject>

class LIBSOUND_EXPORT OutputBackendInterface : public QObject
{
    Q_OBJECT

public:
    explicit OutputBackendInterface(QObject *parent = nullptr);
    virtual ~OutputBackendInterface();
    virtual void setUri(const QString & uri) = 0;
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

