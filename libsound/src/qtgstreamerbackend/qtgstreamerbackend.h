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

#ifndef GSTREAMERBACKEND_H
#define GSTREAMERBACKEND_H

#include "../backendinterface.h"
#include "libsound_export.h"

class CaptureBackendInterface;
class OutputBackendInterface;
class QtGStreamerCaptureBackend;
class QtGStreamerOutputBackend;

class LIBSOUND_EXPORT QtGStreamerBackend : public BackendInterface
{
    Q_OBJECT

public:
    explicit QtGStreamerBackend(QObject *parent, const QList< QVariant >&);
    virtual ~QtGStreamerBackend();

    CaptureBackendInterface * captureBackend() const;
    OutputBackendInterface * outputBackend() const;

private:
    mutable QtGStreamerCaptureBackend *m_captureBackend;
    mutable QtGStreamerOutputBackend *m_outputBackend;
};

#endif


