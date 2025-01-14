/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

//TESTED_COMPONENT=src/multimedia

#include <QtTest/QtTest>
#include <QtGui/QImageReader>
#include <QDebug>

#include <qabstractvideosurface.h>
#include <private/qplatformcamera_p.h>
#include <private/qplatformcameraexposure_p.h>
#include <private/qplatformcamerafocus_p.h>
#include <private/qplatformcameraimagecapture_p.h>
#include <private/qplatformcameraimageprocessing_p.h>
#include <qcamera.h>
#include <qcamerainfo.h>
#include <qcameraimagecapture.h>
#include <qmediacapturesession.h>
#include <qobject.h>
#include <qmediadevicemanager.h>
#include <qmediaencoder.h>

QT_USE_NAMESPACE

/*
 This is the backend conformance test.

 Since it relies on platform media framework and sound hardware
 it may be less stable.
*/

class tst_QCameraBackend: public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();
    void cleanupTestCase();

private slots:
    void testCameraInfo();
    void testCtorWithCameraInfo();
    void testCtorWithPosition();

    void testCameraStates();
    void testCameraStartError();
    void testCameraCapture();
    void testCaptureToBuffer();
    void testCameraCaptureMetadata();
    void testExposureCompensation();
    void testExposureMode();

    void testVideoRecording_data();
    void testVideoRecording();
private:
};

void tst_QCameraBackend::initTestCase()
{
    QCamera camera;
    if (!camera.isAvailable())
        QSKIP("Camera is not available");
}

void tst_QCameraBackend::cleanupTestCase()
{
}

void tst_QCameraBackend::testCameraInfo()
{
    int deviceCount = QMediaDeviceManager::videoInputs().count();
    const QList<QCameraInfo> cameras = QMediaDeviceManager::videoInputs();
    QCOMPARE(cameras.count(), deviceCount);
    if (cameras.isEmpty()) {
        QVERIFY(QMediaDeviceManager::defaultVideoInput().isNull());
        QSKIP("Camera selection is not supported");
    }

    for (const QCameraInfo &info : cameras) {
        QVERIFY(!info.id().isEmpty());
        QVERIFY(!info.description().isEmpty());
    }
}

void tst_QCameraBackend::testCtorWithCameraInfo()
{
    if (QMediaDeviceManager::videoInputs().isEmpty())
        QSKIP("Camera selection not supported");

    {
        QCameraInfo info = QMediaDeviceManager::defaultVideoInput();
        QCamera camera(info);
        QCOMPARE(camera.error(), QCamera::NoError);
        QCOMPARE(camera.cameraInfo(), info);
    }
    {
        QCameraInfo info = QMediaDeviceManager::videoInputs().first();
        QCamera camera(info);
        QCOMPARE(camera.error(), QCamera::NoError);
        QCOMPARE(camera.cameraInfo(), info);
    }
    {
        // loading an invalid CameraInfo should fail
        QCamera *camera = new QCamera(QCameraInfo());
        QCOMPARE(camera->error(), QCamera::CameraError);
        QVERIFY(camera->cameraInfo().isNull());
        delete camera;
    }
}

void tst_QCameraBackend::testCtorWithPosition()
{
    {
        QCamera camera(QCameraInfo::UnspecifiedPosition);
        QCOMPARE(camera.error(), QCamera::NoError);
    }
    {
        QCamera camera(QCameraInfo::FrontFace);
        // even if no camera is available at this position, it should not fail
        // and load the default camera
        QCOMPARE(camera.error(), QCamera::NoError);
    }
    {
        QCamera camera(QCameraInfo::BackFace);
        // even if no camera is available at this position, it should not fail
        // and load the default camera
        QCOMPARE(camera.error(), QCamera::NoError);
    }
}

void tst_QCameraBackend::testCameraStates()
{
    QMediaCaptureSession session;
    QCamera camera;
    QCameraImageCapture imageCapture;
    session.setCamera(&camera);
    session.setImageCapture(&imageCapture);

    QSignalSpy errorSignal(&camera, SIGNAL(errorOccurred(QCamera::Error)));
    QSignalSpy activeChangedSignal(&camera, SIGNAL(activeChanged()));
    QSignalSpy statusChangedSignal(&camera, SIGNAL(statusChanged(QCamera::Status)));

    QCOMPARE(camera.isActive(), false);
    QCOMPARE(camera.status(), QCamera::InactiveStatus);

    camera.start();
    QCOMPARE(camera.isActive(), true);
    QCOMPARE(activeChangedSignal.last().first().value<bool>(), true);
    QTRY_COMPARE(camera.status(), QCamera::ActiveStatus);
    QCOMPARE(statusChangedSignal.last().first().value<QCamera::Status>(), QCamera::ActiveStatus);

    camera.stop();
    QCOMPARE(camera.isActive(), false);
    QCOMPARE(activeChangedSignal.last().first().value<bool>(), false);
    QTRY_COMPARE(camera.status(), QCamera::InactiveStatus);
    QCOMPARE(statusChangedSignal.last().first().value<QCamera::Status>(), QCamera::InactiveStatus);

    QCOMPARE(camera.errorString(), QString());
    QCOMPARE(errorSignal.count(), 0);
}

void tst_QCameraBackend::testCameraStartError()
{
    QMediaCaptureSession session1;
    QMediaCaptureSession session2;
    QCamera camera1(QMediaDeviceManager::defaultVideoInput());
    QCamera camera2(QMediaDeviceManager::defaultVideoInput());
    session1.setCamera(&camera1);
    session2.setCamera(&camera2);
    QSignalSpy errorSpy1(&camera1, &QCamera::errorOccurred);
    QSignalSpy errorSpy2(&camera2, &QCamera::errorOccurred);

    camera1.start();
    camera2.start();

    QCOMPARE(camera1.isActive(), true);
    QTRY_COMPARE(camera1.status(), QCamera::ActiveStatus);
    QCOMPARE(camera1.error(), QCamera::NoError);
    QCOMPARE(camera2.isActive(), false);
    QCOMPARE(camera2.error(), QCamera::CameraError);

    QCOMPARE(errorSpy1.count(), 0);
    QCOMPARE(errorSpy2.count(), 1);
}

void tst_QCameraBackend::testCameraCapture()
{
    QMediaCaptureSession session;
    QCamera camera;
    QCameraImageCapture imageCapture;
    session.setCamera(&camera);
    session.setImageCapture(&imageCapture);

    //prevents camera to flash during the test
    camera.exposure()->setFlashMode(QCameraExposure::FlashOff);

    QVERIFY(!imageCapture.isReadyForCapture());

    QSignalSpy capturedSignal(&imageCapture, SIGNAL(imageCaptured(int,QImage)));
    QSignalSpy savedSignal(&imageCapture, SIGNAL(imageSaved(int,QString)));
    QSignalSpy errorSignal(&imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)));

    imageCapture.capture();
    QTRY_COMPARE(errorSignal.size(), 1);
    QCOMPARE(imageCapture.error(), QCameraImageCapture::NotReadyError);
    QCOMPARE(capturedSignal.size(), 0);
    errorSignal.clear();

    camera.start();

    QTRY_VERIFY(imageCapture.isReadyForCapture());
    QCOMPARE(camera.status(), QCamera::ActiveStatus);
    QCOMPARE(errorSignal.size(), 0);

    int id = imageCapture.capture();

    QTRY_VERIFY(!savedSignal.isEmpty());

    QTRY_COMPARE(capturedSignal.size(), 1);
    QCOMPARE(capturedSignal.last().first().toInt(), id);
    QCOMPARE(errorSignal.size(), 0);
    QCOMPARE(imageCapture.error(), QCameraImageCapture::NoError);

    QCOMPARE(savedSignal.last().first().toInt(), id);
    QString location = savedSignal.last().last().toString();
    QVERIFY(!location.isEmpty());
    QVERIFY(QFileInfo(location).exists());
    QImageReader reader(location);
    reader.setScaledSize(QSize(320,240));
    QVERIFY(!reader.read().isNull());

    QFile(location).remove();
}


void tst_QCameraBackend::testCaptureToBuffer()
{
    QMediaCaptureSession session;
    QCamera camera;
    QCameraImageCapture imageCapture;
    session.setCamera(&camera);
    session.setImageCapture(&imageCapture);

    camera.exposure()->setFlashMode(QCameraExposure::FlashOff);

    camera.setActive(true);

    QTRY_COMPARE(camera.status(), QCamera::ActiveStatus);

    QSignalSpy capturedSignal(&imageCapture, SIGNAL(imageCaptured(int,QImage)));
    QSignalSpy imageAvailableSignal(&imageCapture, SIGNAL(imageAvailable(int,QVideoFrame)));
    QSignalSpy savedSignal(&imageCapture, SIGNAL(imageSaved(int,QString)));
    QSignalSpy errorSignal(&imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)));

    camera.start();
    QTRY_VERIFY(imageCapture.isReadyForCapture());

    int id = imageCapture.captureToBuffer();
    QTRY_VERIFY(!imageAvailableSignal.isEmpty());

    QVERIFY(errorSignal.isEmpty());
    QTRY_VERIFY(!capturedSignal.isEmpty());
    QVERIFY(!imageAvailableSignal.isEmpty());

    QTest::qWait(2000);
    QVERIFY(savedSignal.isEmpty());

    QCOMPARE(capturedSignal.first().first().toInt(), id);
    QCOMPARE(imageAvailableSignal.first().first().toInt(), id);

    QVideoFrame frame = imageAvailableSignal.first().last().value<QVideoFrame>();
    QVERIFY(!frame.image().isNull());

    frame = QVideoFrame();
    capturedSignal.clear();
    imageAvailableSignal.clear();
    savedSignal.clear();

    QTRY_VERIFY(imageCapture.isReadyForCapture());
}

void tst_QCameraBackend::testCameraCaptureMetadata()
{
    QSKIP("Capture metadata is supported only on harmattan");

    QMediaCaptureSession session;
    QCamera camera;
    QCameraImageCapture imageCapture;
    session.setCamera(&camera);
    session.setImageCapture(&imageCapture);

    camera.exposure()->setFlashMode(QCameraExposure::FlashOff);

    QSignalSpy metadataSignal(&imageCapture, SIGNAL(imageMetadataAvailable(int,const QMediaMetaData&)));
    QSignalSpy savedSignal(&imageCapture, SIGNAL(imageSaved(int,QString)));

    camera.start();

    QTRY_VERIFY(imageCapture.isReadyForCapture());

    int id = imageCapture.capture(QString::fromLatin1("/dev/null"));
    QTRY_VERIFY(!savedSignal.isEmpty());
    QVERIFY(!metadataSignal.isEmpty());
    QCOMPARE(metadataSignal.first().first().toInt(), id);
}

void tst_QCameraBackend::testExposureCompensation()
{
    QSKIP("Capture exposure parameters are supported only on mobile platforms");

    QCamera camera;
    QCameraExposure *exposure = camera.exposure();

    QSignalSpy exposureCompensationSignal(exposure, SIGNAL(exposureCompensationChanged(qreal)));

    //it should be possible to set exposure parameters in Unloaded state
    QCOMPARE(exposure->exposureCompensation()+1.0, 1.0);
    exposure->setExposureCompensation(1.0);
    QCOMPARE(exposure->exposureCompensation(), 1.0);
    QTRY_COMPARE(exposureCompensationSignal.count(), 1);
    QCOMPARE(exposureCompensationSignal.last().first().toReal(), 1.0);

    //exposureCompensationChanged should not be emitted when value is not changed
    exposure->setExposureCompensation(1.0);
    QTest::qWait(50);
    QCOMPARE(exposureCompensationSignal.count(), 1);

    //exposure compensation should be preserved during start
    camera.start();
    QTRY_COMPARE(camera.status(), QCamera::ActiveStatus);

    QCOMPARE(exposure->exposureCompensation(), -1.0);

    exposureCompensationSignal.clear();
    exposure->setExposureCompensation(1.0);
    QCOMPARE(exposure->exposureCompensation(), 1.0);
    QTRY_COMPARE(exposureCompensationSignal.count(), 1);
    QCOMPARE(exposureCompensationSignal.last().first().toReal(), 1.0);
}

void tst_QCameraBackend::testExposureMode()
{
    QSKIP("Capture exposure parameters are supported only on mobile platforms");

    QCamera camera;
    QCameraExposure *exposure = camera.exposure();

    QCOMPARE(exposure->exposureMode(), QCameraExposure::ExposureAuto);

    // Night
    exposure->setExposureMode(QCameraExposure::ExposureNight);
    QCOMPARE(exposure->exposureMode(), QCameraExposure::ExposureNight);
    camera.start();
    QTRY_COMPARE(camera.status(), QCamera::ActiveStatus);
    QCOMPARE(exposure->exposureMode(), QCameraExposure::ExposureNight);

    camera.stop();
    QTRY_COMPARE(camera.status(), QCamera::InactiveStatus);

    // Auto
    exposure->setExposureMode(QCameraExposure::ExposureAuto);
    QCOMPARE(exposure->exposureMode(), QCameraExposure::ExposureAuto);
    camera.start();
    QTRY_COMPARE(camera.status(), QCamera::ActiveStatus);
    QCOMPARE(exposure->exposureMode(), QCameraExposure::ExposureAuto);
}

void tst_QCameraBackend::testVideoRecording_data()
{
    QTest::addColumn<QCameraInfo>("device");

    const auto devices = QMediaDeviceManager::videoInputs();

    for (const auto &device : devices) {
        QTest::newRow(device.description().toUtf8())
                << device.id();
    }

    if (devices.isEmpty())
        QTest::newRow("Default device") << QCameraInfo();
}

void tst_QCameraBackend::testVideoRecording()
{
    QFETCH(QCameraInfo, device);

    QMediaCaptureSession session;
    QScopedPointer<QCamera> camera(new QCamera(device));
    session.setCamera(camera.data());

    QMediaEncoder recorder;
    session.setEncoder(&recorder);

    QSignalSpy errorSignal(camera.data(), SIGNAL(errorOccurred(QCamera::Error)));
    QSignalSpy recorderErrorSignal(&recorder, SIGNAL(error(QMediaEncoder::Error)));
    QSignalSpy recorderStatusSignal(&recorder, SIGNAL(statusChanged(QMediaEncoder::Status)));

    QMediaEncoderSettings videoSettings;
    videoSettings.setVideoResolution(320, 240);
    recorder.setEncoderSettings(videoSettings);

    QCOMPARE(recorder.status(), QMediaEncoder::StoppedStatus);

    camera->start();
    QVERIFY(recorder.status() == QMediaEncoder::StartingStatus ||
            recorder.status() == QMediaEncoder::RecordingStatus);
    QCOMPARE(recorderStatusSignal.last().first().value<QMediaEncoder::Status>(), recorder.status());
    QTRY_COMPARE(camera->status(), QCamera::ActiveStatus);
    QTRY_COMPARE(recorder.status(), QMediaEncoder::StoppedStatus);
    QCOMPARE(recorderStatusSignal.last().first().value<QMediaEncoder::Status>(), recorder.status());

    //record 5 seconds clip
    recorder.record();
    QTRY_COMPARE(recorder.status(), QMediaEncoder::RecordingStatus);
    QCOMPARE(recorderStatusSignal.last().first().value<QMediaEncoder::Status>(), recorder.status());
    QTest::qWait(5000);
    recorderStatusSignal.clear();
    recorder.stop();
    bool foundFinalizingStatus = false;
    for (auto &list : recorderStatusSignal) {
        if (list.contains(QVariant(QMediaEncoder::FinalizingStatus))) {
            foundFinalizingStatus = true;
            break;
        }
    }
    QVERIFY(foundFinalizingStatus);
    QTRY_COMPARE(recorder.status(), QMediaEncoder::StoppedStatus);
    QCOMPARE(recorderStatusSignal.last().first().value<QMediaEncoder::Status>(), recorder.status());

    QVERIFY(errorSignal.isEmpty());
    QVERIFY(recorderErrorSignal.isEmpty());

    QString fileName = recorder.actualLocation().toLocalFile();
    QVERIFY(!fileName.isEmpty());

    QVERIFY(QFileInfo(fileName).size() > 0);
    QFile(fileName).remove();

    QTRY_COMPARE(recorder.status(), QMediaEncoder::StoppedStatus);
    QCOMPARE(recorderStatusSignal.last().first().value<QMediaEncoder::Status>(), recorder.status());
}

QTEST_MAIN(tst_QCameraBackend)

#include "tst_qcamerabackend.moc"
