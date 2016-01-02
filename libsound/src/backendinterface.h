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

#ifndef BACKENDINTERFACE_H
#define BACKENDINTERFACE_H

#include "libsound_export.h"
#include <QObject>
#include <QString>

class CaptureBackendInterface;
class OutputBackendInterface;

class LIBSOUND_EXPORT BackendInterface : public QObject
{
    Q_OBJECT

public:
    explicit BackendInterface(const QString &name, QObject *parent = nullptr);
    virtual ~BackendInterface();

    QString name() const;
    virtual CaptureBackendInterface * captureBackend() const = 0;
    virtual OutputBackendInterface * outputBackend() const = 0;

private:
    const QString m_name;
};

#endif
