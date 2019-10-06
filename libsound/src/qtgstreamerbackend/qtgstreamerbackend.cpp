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

#include "qtgstreamerbackend.h"
#include "qtgstreamercapturebackend.h"
#include "qtgstreameroutputbackend.h"
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(BackendFactory, "qtgstreamerbackend.json", registerPlugin<QtGStreamerBackend>();)

QtGStreamerBackend::QtGStreamerBackend(QObject *parent, const QList<QVariant> &)
    : BackendInterface("qtgstreamer", parent)
    , m_captureBackend(nullptr)
    , m_outputBackend(nullptr)
{
}

QtGStreamerBackend::~QtGStreamerBackend()
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

CaptureBackendInterface *QtGStreamerBackend::captureBackend() const
{
    if (!m_captureBackend) {
        m_captureBackend = new QtGStreamerCaptureBackend();
    }
    return m_captureBackend;
}

OutputBackendInterface *QtGStreamerBackend::outputBackend() const
{
    if (!m_outputBackend) {
        m_outputBackend = new QtGStreamerOutputBackend();
    }
    return m_outputBackend;
}

#include "qtgstreamerbackend.moc"
