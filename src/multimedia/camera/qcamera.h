/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QCAMERA_H
#define QCAMERA_H

#include <QtCore/qstringlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qsize.h>
#include <QtCore/qpoint.h>
#include <QtCore/qrect.h>

#include <QtCore/qobject.h>

#include <QtMultimedia/qcameraexposure.h>
#include <QtMultimedia/qcamerafocus.h>
#include <QtMultimedia/qcameraimageprocessing.h>
#include <QtMultimedia/qcamerainfo.h>

#include <QtMultimedia/qmediaenumdebug.h>

QT_BEGIN_NAMESPACE


class QAbstractVideoSurface;
class QCameraInfo;
class QPlatformMediaCaptureSession;
class QMediaCaptureSession;

class QCameraPrivate;
class Q_MULTIMEDIA_EXPORT QCamera : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(QCamera::Status status READ status NOTIFY statusChanged)

    Q_ENUMS(Status)
    Q_ENUMS(Error)
public:
    enum Status {
        UnavailableStatus,
        InactiveStatus,
        StartingStatus,
        StoppingStatus,
        ActiveStatus
    };

    enum Error
    {
        NoError,
        CameraError
    };

    explicit QCamera(QObject *parent = nullptr);
    explicit QCamera(const QCameraInfo& cameraInfo, QObject *parent = nullptr);
    explicit QCamera(QCameraInfo::Position position, QObject *parent = nullptr);
    ~QCamera();

    bool isAvailable() const;
    bool isActive() const;

    Status status() const;

    QMediaCaptureSession *captureSession() const;

    QCameraInfo cameraInfo() const;
    void setCameraInfo(const QCameraInfo &cameraInfo);

    QCameraExposure *exposure() const;
    QCameraFocus *focus() const;
    QCameraImageProcessing *imageProcessing() const;

    Error error() const;
    QString errorString() const;

public Q_SLOTS:
    void setActive(bool active);
    void start() { setActive(true); }
    void stop() { setActive(false); }

Q_SIGNALS:
    void activeChanged(bool);
    void statusChanged(QCamera::Status status);
    void errorOccurred(QCamera::Error);

private:
    void setCaptureSession(QMediaCaptureSession *session);
    friend class QMediaCaptureSession;
    Q_DISABLE_COPY(QCamera)
    Q_DECLARE_PRIVATE(QCamera)
    Q_PRIVATE_SLOT(d_func(), void _q_error(int, const QString &))
    friend class QCameraInfo;
};

QT_END_NAMESPACE

Q_MEDIA_ENUM_DEBUG(QCamera, Status)
Q_MEDIA_ENUM_DEBUG(QCamera, Error)

#endif  // QCAMERA_H
