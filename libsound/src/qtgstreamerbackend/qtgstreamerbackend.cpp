/*
 *  SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
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
