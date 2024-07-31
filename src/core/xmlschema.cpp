/*
    SPDX-FileCopyrightText: 2024 Andreas Cord-Landwehr <cordlandwehr@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "xmlschema.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <cstdio>
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

XmlSchema::XmlSchema()
{
    xmlInitParser();
    xmlLineNumbersDefault(1);
}

XmlSchema::~XmlSchema()
{
    if (mSchema != nullptr) {
        xmlSchemaFree(mSchema);
    }
}

bool XmlSchema::load(const QUrl &path)
{
    //xmlSchemaParserCtxtPtr parserCtxt = xmlSchemaNewParserCtxt(path.toLocalFile().toLocal8Bit());
    QFile file(path.toLocalFile());
    if (file.open(QIODevice::ReadOnly | QFile::Text)) {
        QTextStream in(&file);
        QByteArray buffer = in.readAll().toLocal8Bit();

        xmlSchemaParserCtxtPtr parserCtxt = xmlSchemaNewMemParserCtxt(buffer.data(), buffer.size());
        xmlSchemaSetParserErrors(parserCtxt, xmlErrorFunc, xmlErrorFunc, nullptr);
        mSchema = xmlSchemaParse(parserCtxt);
        xmlSchemaFreeParserCtxt(parserCtxt);
    }

    if (mSchema == nullptr) {
        qWarning() << "Could not parse file" << path;
    }
    return isValid();
}

bool XmlSchema::isValid() const
{
    return mSchema != nullptr;
}

xmlSchemaPtr XmlSchema::data() const
{
    return mSchema;
}
