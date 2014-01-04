/*
 *  Copyright 2013  Andreas Cord-Landwehr <cordlandwehr@kde.org>
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

#include "recorder.h"
#include "capturedevicecontroller.h"

#include <QList>
#include <KDebug>
#include <KUrl>
#include <QString>

Recorder::Recorder(QObject *parent)
    : QObject(parent)
    , m_soundFile(QString())
    , m_state(StoppedState)
{

}

Recorder::~Recorder()
{
    // nothing to do
}

void Recorder::setSoundFile(const KUrl &fileUrl)
{
    if (!fileUrl.isValid() || fileUrl.isEmpty()) {
        kWarning() << "Not setting empty sound file path.";
        return;
    }
    m_soundFile = fileUrl;
    emit soundFileChanged();
}

void Recorder::setSoundFile(const QString& fileUrl)
{
    CaptureDeviceController::self().stopCapture();
    setSoundFile(KUrl::fromLocalFile(fileUrl));
}

QString Recorder::soundFile() const
{
    return m_soundFile.toLocalFile();
}

Recorder::CaptureState Recorder::state() const
{
    return m_state;
}

void Recorder::startCapture()
{
    if (m_soundFile.isEmpty()) {
        kError() << "Abort capture, no output file set";
        return;
    }
    kDebug() << this << "Capture to file " << m_soundFile.toLocalFile();
    m_state = RecordingState;
    CaptureDeviceController::self().startCapture(m_soundFile.toLocalFile());
    connect(&CaptureDeviceController::self(), SIGNAL(captureStopped()), this, SLOT(updateState()));
    emit stateChanged();
}

void Recorder::stop()
{
    CaptureDeviceController::self().stopCapture();
    CaptureDeviceController::self().disconnect();
    emit stateChanged();
}

void Recorder::updateState()
{
    if (!CaptureDeviceController::self().state() == CaptureDeviceController::StoppedState) {
        return;
    }
    if (state() == RecordingState) {
        m_state = StoppedState;
        emit stateChanged();
    }
    CaptureDeviceController::self().disconnect();
}
