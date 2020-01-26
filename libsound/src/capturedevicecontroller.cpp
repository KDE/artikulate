/*
 *  Copyright 2013-2014  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "capturedevicecontroller.h"
#include "backendinterface.h"
#include "capturebackendinterface.h"
#include "libsound_debug.h"

#include <QCoreApplication>
#include <QPluginLoader>
#include <QStringList>

#include <KPluginFactory>
#include <KPluginLoader>
#include <KPluginMetaData>

/**
 * \class CaptureDeviceControllerPrivate
 * \internal
 *
 * This is the private data class for \see CaptureDeviceController.
 * Note that -- even if the CaptureDeviceController is constructed before its first call -- all
 * devices get only configured by first instantiation of CaptureDeviceController with a call
 * to CaptureDeviceControllerPrivate::lazyInit(), called in CaptureDeviceController::self().
 */
class CaptureDeviceControllerPrivate
{
public:
    CaptureDeviceControllerPrivate(QObject *parent)
        : m_parent(parent)
        , m_backend(nullptr)
        , m_initialized(false)
    {
        QStringList dirsToCheck;
        foreach (const QString &directory, QCoreApplication::libraryPaths()) {
            dirsToCheck << directory + "/artikulate/libsound";
        }
        // load plugins
        QPluginLoader loader;
        foreach (const QString &dir, dirsToCheck) {
            QVector<KPluginMetaData> metadataList = KPluginLoader::findPlugins(dir, [=](const KPluginMetaData &data) { return data.serviceTypes().contains(QLatin1String("artikulate/libsound/backend")); });

            foreach (const auto &metadata, metadataList) {
                loader.setFileName(metadata.fileName());
                qCDebug(LIBSOUND_LOG) << "Load Plugin: " << metadata.name();
                if (!loader.load()) {
                    qCCritical(LIBSOUND_LOG) << "Error while loading plugin: " << metadata.name();
                }
                KPluginFactory *factory = KPluginLoader(loader.fileName()).factory();
                if (!factory) {
                    qCCritical(LIBSOUND_LOG) << "Could not load plugin: " << metadata.name();
                    continue;
                }
                BackendInterface *plugin = factory->create<BackendInterface>(parent, QList<QVariant>());
                if (plugin->captureBackend()) {
                    m_backendList.append(plugin->captureBackend());
                }
            }
        }
        if (!m_backend && !m_backendList.isEmpty()) {
            m_backend = m_backendList.first();
        }
    }

    ~CaptureDeviceControllerPrivate()
    {
        delete m_backend;
        m_backend = nullptr;
    }

    void lazyInit()
    {
        if (m_initialized) {
            return;
        }
        // TODO currently nothing to do
        m_initialized = true;
    }

    CaptureBackendInterface *backend() const
    {
        Q_ASSERT(m_backend);
        return m_backend;
    }

    QObject *m_parent;
    CaptureBackendInterface *m_backend;
    QList<CaptureBackendInterface *> m_backendList;
    bool m_initialized;
};

CaptureDeviceController::CaptureDeviceController()
    : d(new CaptureDeviceControllerPrivate(this))
{
}

CaptureDeviceController::~CaptureDeviceController()
{
}

CaptureDeviceController &CaptureDeviceController::self()
{
    static CaptureDeviceController instance;
    instance.d->lazyInit();
    return instance;
}

void CaptureDeviceController::startCapture(const QString &filePath)
{
    d->backend()->startCapture(filePath);
    emit captureStarted();
}

void CaptureDeviceController::stopCapture()
{
    d->backend()->stopCapture();
    emit captureStopped();
}

void CaptureDeviceController::setDevice(const QString &deviceIdentifier)
{
    d->backend()->setDevice(deviceIdentifier);
}

QList<QString> CaptureDeviceController::devices() const
{
    return d->backend()->devices();
}

CaptureDeviceController::State CaptureDeviceController::state() const
{
    return d->backend()->captureState();
}
