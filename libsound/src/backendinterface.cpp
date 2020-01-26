/*
 *  Copyright 2016  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "backendinterface.h"

BackendInterface::BackendInterface(const QString &name, QObject *parent)
    : QObject(parent)
    , m_name(name)
{
}

BackendInterface::~BackendInterface()
{
}

QString BackendInterface::name() const
{
    return m_name;
}
