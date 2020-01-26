/*
 *  Copyright 2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
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
    explicit QtMultimediaBackend(QObject *parent, const QList<QVariant> &);
    virtual ~QtMultimediaBackend();

    CaptureBackendInterface *captureBackend() const override;
    OutputBackendInterface *outputBackend() const override;

private:
    QtMultimediaCaptureBackend *m_captureBackend;
    QtMultimediaOutputBackend *m_outputBackend;
};

#endif
