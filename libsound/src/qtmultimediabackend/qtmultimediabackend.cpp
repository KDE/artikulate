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

#include "qtmultimediabackend.h"
#include "qtmultimediacapturebackend.h"
#include "qtmultimediaoutputbackend.h"
#include "libsound_export.h"
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON( BackendFactory,
                            "qtmultimediabackend.json",
                            registerPlugin<QtMultimediaBackend>();)

QtMultimediaBackend::QtMultimediaBackend(QObject *parent, const QList< QVariant >&)
    : BackendInterface("qtmultimedia", parent)
    , m_captureBackend(nullptr)
    , m_outputBackend(nullptr)
{

}

QtMultimediaBackend::~QtMultimediaBackend()
{
    if (m_captureBackend) {
        m_captureBackend->deleteLater();
        m_captureBackend = nullptr;
    }
    if (m_outputBackend) {
        m_outputBackend->deleteLater();
        m_outputBackend = nullptr;
    }
}

CaptureBackendInterface * QtMultimediaBackend::captureBackend() const
{
    if (!m_captureBackend) {
        m_captureBackend = new QtMultimediaCaptureBackend();
    }
    return m_captureBackend;
}

OutputBackendInterface * QtMultimediaBackend::outputBackend() const
{
    if (!m_outputBackend) {
        m_outputBackend = new QtMultimediaOutputBackend();
    }
    return m_outputBackend;
}

#include "qtmultimediabackend.moc"
