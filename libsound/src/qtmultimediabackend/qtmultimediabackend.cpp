/*
 *  Copyright 2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "qtmultimediabackend.h"
#include "qtmultimediacapturebackend.h"
#include "qtmultimediaoutputbackend.h"
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(BackendFactory, "qtmultimediabackend.json", registerPlugin<QtMultimediaBackend>();)

QtMultimediaBackend::QtMultimediaBackend(QObject *parent, const QList<QVariant> &)
    : BackendInterface(QStringLiteral("qtmultimedia"), parent)
    , m_captureBackend(new QtMultimediaCaptureBackend(this))
    , m_outputBackend(new QtMultimediaOutputBackend(this))
{
}

QtMultimediaBackend::~QtMultimediaBackend()
{
}

CaptureBackendInterface *QtMultimediaBackend::captureBackend() const
{
    return m_captureBackend;
}

OutputBackendInterface *QtMultimediaBackend::outputBackend() const
{
    return m_outputBackend;
}

#include "qtmultimediabackend.moc"
