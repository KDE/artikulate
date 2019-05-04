/*
 *  Copyright 2019  Andreas Cord-Landwehr <cordlandwehr@kde.org>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License or (at your option) version 3 or any later version
 *  accepted by the membership of KDE e.V. (or its successor approved
 *  by the membership of KDE e.V.), which shall act as a proxy
 *  defined in Section 14 of version 3 of the license.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "editablecourseresource.h"
#include "courseparser.h"
#include "artikulate_debug.h"
#include "core/unit.h"
#include "core/phrase.h"
#include "core/phoneme.h"

#include <QDomDocument>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <KTar>

EditableCourseResource::EditableCourseResource(const QUrl &path, IResourceRepository *repository)
    : m_course(new CourseResource(path, repository))
{
}

QString EditableCourseResource::id() const
{
    return m_course->id();
}

QString EditableCourseResource::foreignId() const
{
    return m_course->foreignId();
}

QString EditableCourseResource::title() const
{
    return m_course->title();
}

QString EditableCourseResource::i18nTitle() const
{
    return m_course->i18nTitle();
}

QString EditableCourseResource::description() const
{
    return m_course->description();
}

Language * EditableCourseResource::language() const
{
    return m_course->language();
}

QList<Unit *> EditableCourseResource::unitList()
{
    return m_course->unitList();
}

QUrl EditableCourseResource::file() const
{
    return m_course->file();
}

void EditableCourseResource::sync()
{
    Q_ASSERT(file().isValid());
    Q_ASSERT(file().isLocalFile());
    Q_ASSERT(!file().isEmpty());

     // not writing back if not modified
     if (!m_modified) {
         qCDebug(ARTIKULATE_LOG()) << "Aborting sync, course was not modified.";
         return;
     }

    // write back to file
    // create directories if necessary
    QFileInfo info(file().adjusted(QUrl::RemoveFilename|QUrl::StripTrailingSlash).path());
    if (!info.exists()) {
        qCDebug(ARTIKULATE_LOG) << "create xml output file directory, not existing";
        QDir dir;
        dir.mkpath(file().adjusted(QUrl::RemoveFilename|QUrl::StripTrailingSlash).path());
    }

    //TODO port to KSaveFile
    QFile file(EditableCourseResource::file().toLocalFile());
    if (!file.open(QIODevice::WriteOnly)) {
        qCWarning(ARTIKULATE_LOG) << "Unable to open file " << file.fileName() << " in write mode, aborting.";
        return;
    }

    file.write(CourseParser::serializedDocument(m_course.get(), false).toByteArray());
    return;
}
