/*
    SPDX-FileCopyrightText: 2016 Andreas Cord-Landwehr <cordlandwehr@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "outputbackendinterface.h"

OutputBackendInterface::OutputBackendInterface(QObject *parent)
    : QObject(parent)
{
}

OutputBackendInterface::~OutputBackendInterface()
{
}
