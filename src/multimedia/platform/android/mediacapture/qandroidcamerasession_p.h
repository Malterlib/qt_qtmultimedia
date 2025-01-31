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

#ifndef QANDROIDCAMERASESSION_H
#define QANDROIDCAMERASESSION_H

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

#include <qcamera.h>
#include <qmediaencodersettings.h>
#include <QCameraImageCapture>
#include <QSet>
#include <QMutex>
#include <private/qmediastoragelocation_p.h>
#include "androidcamera_p.h"

QT_BEGIN_NAMESPACE

class QAndroidVideoOutput;
class QAndroidCameraExposureControl;
class QAndroidCameraFocusControl;
class QAndroidCameraImageProcessingControl;
class QAndroidCameraVideoRendererControl;

class QAndroidCameraSession : public QObject
{
    Q_OBJECT
public:
    explicit QAndroidCameraSession(QObject *parent = 0);
    ~QAndroidCameraSession();

    static const QList<QCameraInfo> &availableCameras();

    void setSelectedCamera(int cameraId) { m_selectedCamera = cameraId; }
    AndroidCamera *camera() const { return m_camera; }

    bool isActive() const { return m_active; }
    void setActive(bool active);

    QCamera::Status status() const { return m_status; }

    void applyResolution(const QSize &captureSize = QSize(), bool restartPreview = true);

    QAndroidVideoOutput *videoOutput() const { return m_videoOutput; }
    void setVideoOutput(QAndroidVideoOutput *output);

    QList<QSize> getSupportedPreviewSizes() const;
    QList<QVideoFrame::PixelFormat> getSupportedPixelFormats() const;
    QList<AndroidCamera::FpsRange> getSupportedPreviewFpsRange() const;

    QImageEncoderSettings imageSettings() const { return m_actualImageSettings; }
    void setImageSettings(const QImageEncoderSettings &settings);

    bool isReadyForCapture() const;
    void setReadyForCapture(bool ready);
    int capture(const QString &fileName);

    int currentCameraRotation() const;

    void setPreviewFormat(AndroidCamera::ImageFormat format);

    struct PreviewCallback
    {
        virtual void onFrameAvailable(const QVideoFrame &frame) = 0;
    };
    void setPreviewCallback(PreviewCallback *callback);
    bool requestRecordingPermission();

    QAndroidCameraFocusControl *focusControl() { return m_cameraFocusControl; }
    QAndroidCameraExposureControl *exposureControl() { return m_cameraExposureControl; }
    QAndroidCameraImageProcessingControl *imageProcessingControl() { return m_cameraImageProcessingControl; }

    void setVideoSurface(QAbstractVideoSurface *surface);

Q_SIGNALS:
    void statusChanged(QCamera::Status status);
    void activeChanged(bool);
    void error(int error, const QString &errorString);
    void opened();

    void readyForCaptureChanged(bool);
    void imageExposed(int id);
    void imageCaptured(int id, const QImage &preview);
    void imageMetadataAvailable(int id, const QString &key, const QVariant &value);
    void imageAvailable(int id, const QVideoFrame &buffer);
    void imageSaved(int id, const QString &fileName);
    void imageCaptureError(int id, int error, const QString &errorString);

private Q_SLOTS:
    void onVideoOutputReady(bool ready);

    void onApplicationStateChanged(Qt::ApplicationState state);

    void onCameraTakePictureFailed();
    void onCameraPictureExposed();
    void onCameraPictureCaptured(const QByteArray &data);
    void onLastPreviewFrameFetched(const QVideoFrame &frame);
    void onNewPreviewFrame(const QVideoFrame &frame);
    void onCameraPreviewStarted();
    void onCameraPreviewFailedToStart();
    void onCameraPreviewStopped();

private:
    static void updateAvailableCameras();

    bool open();
    void close();

    bool startPreview();
    void stopPreview();

    void applyImageSettings();

    void processPreviewImage(int id, const QVideoFrame &frame, int rotation);
    void processCapturedImage(int id,
                              const QByteArray &data,
                              const QSize &resolution,
                              bool captureToBuffer,
                              const QString &fileName);

    static QVideoFrame::PixelFormat QtPixelFormatFromAndroidImageFormat(AndroidCamera::ImageFormat);
    static AndroidCamera::ImageFormat AndroidImageFormatFromQtPixelFormat(QVideoFrame::PixelFormat);

    void setActiveHelper(bool active);

    int m_selectedCamera;
    AndroidCamera *m_camera;
    int m_nativeOrientation;
    QAndroidVideoOutput *m_videoOutput;

    bool m_active = false;
    int m_savedState = -1;
    QCamera::Status m_status;
    bool m_previewStarted;

    QAndroidCameraVideoRendererControl *m_renderer = nullptr;

    QAndroidCameraExposureControl *m_cameraExposureControl;
    QAndroidCameraFocusControl *m_cameraFocusControl;
    QAndroidCameraImageProcessingControl *m_cameraImageProcessingControl;

    QImageEncoderSettings m_requestedImageSettings;
    QImageEncoderSettings m_actualImageSettings;
    int m_lastImageCaptureId;
    bool m_readyForCapture;
    int m_currentImageCaptureId;
    QString m_currentImageCaptureFileName;

    QMediaStorageLocation m_mediaStorageLocation;

    QMutex m_videoFrameCallbackMutex;
    PreviewCallback *m_previewCallback;
    bool m_keepActive;
};

QT_END_NAMESPACE

#endif // QANDROIDCAMERASESSION_H
