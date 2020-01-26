/*
 *  Copyright 2013-2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#ifndef COURSEPARSER_H
#define COURSEPARSER_H

#include "artikulatecore_export.h"
#include <QVector>
#include <memory>

class IEditableCourse;
class IEditablePhrase;
class IUnit;
class Unit;
class Phrase;
class Phoneme;
class IResourceRepository;
class QXmlSchema;
class QJSonDocument;
class QDomDocument;
class QDomElement;
class QXmlStreamReader;
class QString;
class QUrl;

class ARTIKULATECORE_EXPORT CourseParser
{
public:
    /**
     * Load XSD file given by its file name (without ".xsd" suffix). The method searches exclusively
     * the standard install dir for XSD files in subdirectory "schemes/".
     *
     * \param schemeName name of the Xml schema without suffix
     * \return loaded XML Schema
     */
    static QXmlSchema loadXmlSchema(const QString &schemeName);

    /**
     * Load XML file given by \p file that confirms with XML schema \p scheme.
     *
     * \param path is the path to the XML file to be loaded
     * \param scheme is the XML schema describing the DOM
     * \return the loaded DOM document
     */
    static QDomDocument loadDomDocument(const QUrl &path, const QXmlSchema &schema);

    /**
     * @brief Parse unit from XML file
     * @param path the path to the file
     * @param phonemes list of phonemes that are generated for the language of the unit
     * @param skipIncomplete if set to true, empty units and phrases without native sound files are skipped
     * @return parsed unit
     */
    static std::vector<std::shared_ptr<Unit>> parseUnits(const QUrl &path, QVector<std::shared_ptr<Phoneme>> phonemes = QVector<std::shared_ptr<Phoneme>>(), bool skipIncomplete = false);

    static QDomDocument serializedDocument(std::shared_ptr<IEditableCourse> course, bool trainingExport);
    static QDomElement serializedPhrase(std::shared_ptr<IEditablePhrase> phrase, QDomDocument &document);
    static bool exportCourseToGhnsPackage(std::shared_ptr<IEditableCourse> course, const QString &exportPath);

private:
    static std::shared_ptr<Unit> parseUnit(QXmlStreamReader &xml, const QUrl &path, QVector<std::shared_ptr<Phoneme>> phonemes, bool skipIncomplete, bool &ok);
    static std::shared_ptr<Phrase> parsePhrase(QXmlStreamReader &xml, const QUrl &path, QVector<std::shared_ptr<Phoneme>> phonemes, bool &ok);
    static QStringList parsePhonemeIds(QXmlStreamReader &xml, bool &ok);
    static QString parseElement(QXmlStreamReader &xml, bool &ok);
};

#endif
