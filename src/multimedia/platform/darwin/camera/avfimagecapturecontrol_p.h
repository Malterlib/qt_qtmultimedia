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

#ifndef AVFIMAGECAPTURECONTROL_H
#define AVFIMAGECAPTURECONTROL_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#import <AVFoundation/AVFoundation.h>

#include <QtCore/qqueue.h>
#include <QtCore/qsemaphore.h>
#include <QtCore/qsharedpointer.h>
#include <private/qplatformcameraimagecapture_p.h>
#include "avfcamerasession_p.h"
#include "avfstoragelocation_p.h"

QT_BEGIN_NAMESPACE

class AVFImageCaptureControl : public QPlatformCameraImageCapture
{
Q_OBJECT
public:
    struct CaptureRequest {
        int captureId;
        QSharedPointer<QSemaphore> previewReady;
    };

    AVFImageCaptureControl(AVFCameraService *service, QObject *parent = nullptr);
    ~AVFImageCaptureControl();

    bool isReadyForCapture() const override;

    AVCaptureStillImageOutput *stillImageOutput() const {return m_stillImageOutput;}

    int doCapture(const QString &fileName);
    int capture(const QString &fileName) override;
    int captureToBuffer() override;

    QImageEncoderSettings imageSettings() const override;
    void setImageSettings(const QImageEncoderSettings &settings) override;
    bool applySettings();

private Q_SLOTS:
    void updateCaptureConnection();
    void updateReadyStatus();
    void onNewViewfinderFrame(const QVideoFrame &frame);

private:
    void makeCapturePreview(CaptureRequest request, const QVideoFrame &frame, int rotation);
    bool videoCaptureDeviceIsValid() const;

    AVFCameraService *m_service;
    AVFCameraSession *m_session;
    AVFCameraControl *m_cameraControl;
    bool m_ready;
    int m_lastCaptureId;
    AVCaptureStillImageOutput *m_stillImageOutput;
    AVCaptureConnection *m_videoConnection;
    AVFStorageLocation m_storageLocation;

    QMutex m_requestsMutex;
    QQueue<CaptureRequest> m_captureRequests;
    QImageEncoderSettings m_settings;
};

Q_DECLARE_TYPEINFO(AVFImageCaptureControl::CaptureRequest, Q_PRIMITIVE_TYPE);

QT_END_NAMESPACE

#endif
