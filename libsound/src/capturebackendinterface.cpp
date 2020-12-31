/*
    SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "capturebackendinterface.h"

CaptureBackendInterface::CaptureBackendInterface(QObject *parent)
    : QObject(parent)
{
}

CaptureBackendInterface::~CaptureBackendInterface()
{
}
