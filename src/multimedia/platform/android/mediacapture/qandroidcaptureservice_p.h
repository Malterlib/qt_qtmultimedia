/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Copyright (C) 2016 Ruslan Baratov
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

#ifndef QANDROIDCAPTURESERVICE_H
#define QANDROIDCAPTURESERVICE_H

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

#include <private/qplatformmediacapture_p.h>
#include <private/qplatformmediaintegration_p.h>

QT_BEGIN_NAMESPACE

class QAndroidMediaEncoder;
class QAndroidCaptureSession;
class QAndroidCameraControl;
class QAndroidCameraSession;
class QAndroidCameraImageCaptureControl;

class QAndroidCaptureService : public QPlatformMediaCaptureSession
{
    Q_OBJECT

public:
    explicit QAndroidCaptureService(QMediaRecorder::CaptureMode mode);
    virtual ~QAndroidCaptureService();

    QPlatformCamera *cameraControl() override;
    QPlatformCameraImageCapture *imageCaptureControl() override;
    QPlatformMediaEncoder *mediaEncoder() override;

    bool isMuted() const override;
    void setMuted(bool muted) override;
    qreal volume() const override;
    void setVolume(qreal volume) override;
    QAudioDeviceInfo audioInput() const override;
    bool setAudioInput(const QAudioDeviceInfo &id) override;

    void setVideoPreview(QAbstractVideoSurface *surface) override;

    bool m_videoEnabled = false;

    QAndroidMediaEncoder *m_recorderControl;
    QAndroidCaptureSession *m_captureSession;
    QAndroidCameraControl *m_cameraControl;
    QAndroidCameraSession *m_cameraSession;
    QAndroidCameraImageCaptureControl *m_imageCaptureControl;
};

QT_END_NAMESPACE

#endif // QANDROIDCAPTURESERVICE_H
