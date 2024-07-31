/*
    SPDX-FileCopyrightText: 2024 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "xmlschemavalidator.h"
#include <QDebug>
#include <QFile>
#include <libxml/xmlschemastypes.h>
#include <libxml/parser.h>
#include <libxml/xmlschemas.h>

namespace {
void xmlErrorFunc(void *ctx, const char *msg, ...) {
    Q_UNUSED(ctx);
    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
}
}

XmlSchemaValidator::XmlSchemaValidator(const XmlSchema &schema)
    : mSchema(schema)
{
}

bool XmlSchemaValidator::validate(const QUrl &path)
{
    bool success = true;
    xmlDocPtr doc{ nullptr };

    if (!mSchema.isValid()) {
        qCritical() << "Aborting validation with invalid scheme";
        success = false;
    }

    // load the XML document
    if (success) {
        QFile file(path.toLocalFile());
        if (file.open(QIODevice::ReadOnly | QFile::Text)) {
            QTextStream in(&file);
            QByteArray buffer = in.readAll().toLocal8Bit();

            doc = xmlReadMemory(buffer.data(), buffer.size(), nullptr, nullptr, XML_PARSE_NONET);
            if (doc == nullptr) {
                qWarning() << "Could not read XML file:" << path;
                success = false;
            }
        }
    }

    if (success) {
        // Create a validation context
        xmlSchemaValidCtxtPtr validCtxt = xmlSchemaNewValidCtxt(mSchema.data());
        xmlSchemaSetValidErrors(validCtxt, xmlErrorFunc, xmlErrorFunc, NULL);
        if (xmlSchemaValidateDoc(validCtxt, doc) == 0) {
            success = false;
        } else {
            qWarning() << "XML is not valid.";
        }
        xmlSchemaFreeValidCtxt(validCtxt);
        xmlCleanupParser();
        xmlMemoryDump();
    }

    xmlFreeDoc(doc);

    return success;
}
