/*
 *  SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
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
    virtual CaptureBackendInterface *captureBackend() const = 0;
    virtual OutputBackendInterface *outputBackend() const = 0;

private:
    const QString m_name;
};

#endif
