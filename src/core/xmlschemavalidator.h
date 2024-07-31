/*
    SPDX-FileCopyrightText: 2024 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef XMLSCHEMEVALIDATOR_H
#define XMLSCHEMEVALIDATOR_H

#include "artikulatecore_export.h"
#include <QUrl>
#include <core/xmlschema.h>

class ARTIKULATECORE_EXPORT XmlSchemaValidator
{
public:
    XmlSchemaValidator(const XmlSchema &scheme);
    bool validate(const QUrl &path);

private:
    const XmlSchema &mSchema;
};

#endif // XMLSCHEMAVALIDATOR_H
