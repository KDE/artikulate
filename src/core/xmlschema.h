/*
    SPDX-FileCopyrightText: 2024 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef XMLSCHEMA_H
#define XMLSCHEMA_H

#include "artikulatecore_export.h"
#include <QUrl>
#include <libxml/xmlschemastypes.h>

class ARTIKULATECORE_EXPORT XmlSchema
{
public:
    XmlSchema();
    ~XmlSchema();
    bool load(const QUrl &path);
    bool isValid() const;
    xmlSchemaPtr data() const;

private:
    xmlSchemaPtr mSchema{nullptr};
};

#endif // XMLSCHEMA_H
