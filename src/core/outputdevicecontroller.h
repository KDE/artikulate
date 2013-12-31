/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#ifndef OUTPUTDEVICECONTROLLER_H
#define OUTPUTDEVICECONTROLLER_H

#include "artikulatecore_export.h"

#include <QObject>
#include <Phonon/AudioOutput>
#include <Phonon/MediaObject>

class OutputDeviceControllerPrivate;
class KUrl;

/**
 * \class OutputDeviceController
 *
 * This singelton class provides a controller for the sound output device.
 */
class ARTIKULATELIB_EXPORT OutputDeviceController : public QObject
{
    Q_OBJECT

public:
    /**
     * Returns self reference to the controller. First call of this method initializes
     * output device controller.
     *
     * \return self reference
     */
    static OutputDeviceController & self();

    void play(const QString &filePath);
    void play(const KUrl &filePath);
    Phonon::State state() const;
    void stop();
    QString currentSource() const;

public Q_SLOTS:


Q_SIGNALS:
    void started();
    void stopped();

private Q_SLOTS:
    void updateState();

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
