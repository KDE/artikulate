/*
 *  Copyright 2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef GSTREAMERBACKEND_H
#define GSTREAMERBACKEND_H

#include "../backendinterface.h"

class CaptureBackendInterface;
class OutputBackendInterface;
class QtGStreamerCaptureBackend;
class QtGStreamerOutputBackend;

class QtGStreamerBackend : public BackendInterface
{
    Q_OBJECT

public:
    explicit QtGStreamerBackend(QObject *parent, const QList<QVariant> &);
    virtual ~QtGStreamerBackend();

    CaptureBackendInterface *captureBackend() const;
    OutputBackendInterface *outputBackend() const;

private:
    mutable QtGStreamerCaptureBackend *m_captureBackend;
    mutable QtGStreamerOutputBackend *m_outputBackend;
};

#endif
