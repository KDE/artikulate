/*
    SPDX-FileCopyrightText: 2013-2014 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "outputdevicecontroller.h"
#include "backendinterface.h"
#include "outputbackendinterface.h"

#include <QCoreApplication>
#include <QPluginLoader>
#include <QUrl>

#include <KPluginFactory>
#include <KPluginLoader>
#include <KPluginMetaData>

#include "libsound_debug.h"

/**
 * \class OutputDeviceControllerPrivate
 * \internal
 *
 * This is the private data class for \see OutputDeviceController.
 * Note that -- even if the OutputDeviceController is constructed before its first call -- all
 * devices get only configured by first instantiation of OutputDeviceController with a call
 * to OutputDeviceControllerPrivate::lazyInit(), called in OutputDeviceController::self().
 */
class OutputDeviceControllerPrivate
{
public:
    OutputDeviceControllerPrivate(OutputDeviceController *parent)
        : m_parent(parent)
        , m_backend(nullptr)
        , m_volume(0)
        , m_initialized(false)
    {
        const QVector<KPluginMetaData> metadataList = KPluginLoader::findPlugins(QStringLiteral("artikulate/libsound"));
        for (const auto &metadata : metadataList) {
            qCDebug(LIBSOUND_LOG) << "Load Plugin: " << metadata.name();
            KPluginFactory *factory = KPluginLoader(metadata.fileName()).factory();
            if (!factory) {
                qCCritical(LIBSOUND_LOG) << "Could not load plugin:" << metadata.name();
                continue;
            }
            BackendInterface *plugin = factory->create<BackendInterface>(parent, QList<QVariant>());
            if (plugin->outputBackend()) {
                m_backendList.append(plugin->outputBackend());
            }
        }
        if (!m_backend && !m_backendList.isEmpty()) {
            m_backend = m_backendList.first();
        }
    }

    ~OutputDeviceControllerPrivate()
    {
        delete m_backend;
    }

    void lazyInit()
    {
        if (m_initialized) {
            return;
        }
        m_parent->connect(m_backend, &OutputBackendInterface::stateChanged, m_parent, &OutputDeviceController::emitChangedState);
        m_volume = m_backend->volume();
        m_initialized = true;
    }

    OutputBackendInterface *backend() const
    {
        Q_ASSERT(m_backend);
        return m_backend;
    }

    OutputDeviceController *m_parent;
    OutputBackendInterface *m_backend;
    QList<OutputBackendInterface *> m_backendList;
    int m_volume; // volume as cubic value
    bool m_initialized;
};

OutputDeviceController::OutputDeviceController()
    : d(new OutputDeviceControllerPrivate(this))
{
}

OutputDeviceController::~OutputDeviceController()
{
}

OutputDeviceController &OutputDeviceController::self()
{
    static OutputDeviceController instance;
    instance.d->lazyInit();
    return instance;
}

void OutputDeviceController::play(const QString &filePath)
{
    d->backend()->setUri(filePath);
    d->backend()->setVolume(d->m_volume);
    d->backend()->play();
    emit started();
}

void OutputDeviceController::play(const QUrl &filePath)
{
    play(filePath.toLocalFile());
}

void OutputDeviceController::stop()
{
    d->backend()->stop();
    emit stopped();
}

OutputDeviceController::State OutputDeviceController::state() const
{
    return d->backend()->state();
}

void OutputDeviceController::setVolume(int volume)
{
    // backend only accepts volume, when there is a pipeline
    // store value here and set it when playing
    d->backend()->setVolume(volume);
    d->m_volume = volume;
}

int OutputDeviceController::volume() const
{
    return d->backend()->volume();
}

void OutputDeviceController::emitChangedState()
{
    if (state() == OutputDeviceController::StoppedState) {
        emit stopped();
        return;
    }
    if (state() == OutputDeviceController::PlayingState) {
        emit started();
        return;
    }
}
