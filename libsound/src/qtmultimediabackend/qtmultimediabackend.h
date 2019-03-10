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

#ifndef QTMULTIMEDIABACKEND_H
#define QTMULTIMEDIABACKEND_H

#include "../backendinterface.h"

class CaptureBackendInterface;
class OutputBackendInterface;
class QtMultimediaCaptureBackend;
class QtMultimediaOutputBackend;

class QtMultimediaBackend : public BackendInterface
{
    Q_OBJECT

public:
    explicit QtMultimediaBackend(QObject *parent, const QList< QVariant >&);
    virtual ~QtMultimediaBackend();

    CaptureBackendInterface * captureBackend() const Q_DECL_OVERRIDE;
    OutputBackendInterface * outputBackend() const Q_DECL_OVERRIDE;

private:
    QtMultimediaCaptureBackend *m_captureBackend;
    QtMultimediaOutputBackend *m_outputBackend;
};

#endif


