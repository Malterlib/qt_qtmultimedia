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

#include <QtTest/QtTest>
#include <QDebug>

#include <qaudioformat.h>

#include <qmediaencoder.h>
#include <private/qplatformmediaencoder_p.h>
#include <qaudiodeviceinfo.h>
#include <qaudioinput.h>
#include <qmediacapturesession.h>

//TESTED_COMPONENT=src/multimedia

#include "mockmediarecorderservice.h"
#include "qmockintegration_p.h"

QT_USE_NAMESPACE

class tst_QAudioRecorder: public QObject
{
    Q_OBJECT

public slots:
    void init();
    void cleanup();

private slots:
    void testNullService();
    void testNullControl();
    void testAudioSource();
    void testDevices();
    void testAvailability();

private:
    QMediaEncoder *audiosource = nullptr;
    QMockIntegration *mockIntegration;
};

void tst_QAudioRecorder::init()
{
    mockIntegration = new QMockIntegration;
    audiosource = nullptr;
}

void tst_QAudioRecorder::cleanup()
{
    delete mockIntegration;
    delete audiosource;
    mockIntegration = nullptr;
    audiosource = nullptr;
}

void tst_QAudioRecorder::testNullService()
{
    mockIntegration->setFlags(QMockIntegration::NoCaptureInterface);
    QMediaRecorder source;

    QVERIFY(!source.isAvailable());

    QCOMPARE(source.audioInput(), QAudioDeviceInfo());
}


void tst_QAudioRecorder::testNullControl()
{
    QMediaRecorder source;
    auto *service = mockIntegration->lastCaptureService();
    service->hasControls = false;

    QVERIFY(!source.isAvailable());

    QCOMPARE(source.audioInput(), QAudioDeviceInfo());

    QSignalSpy deviceNameSpy(&source, SIGNAL(audioInputChanged(QString)));

    source.setAudioInput(QAudioDeviceInfo());
    QCOMPARE(deviceNameSpy.count(), 0);
}

void tst_QAudioRecorder::testAudioSource()
{
    QMediaCaptureSession session;
    audiosource = new QMediaEncoder;
    session.setEncoder(audiosource);

    QCOMPARE(session.camera(), nullptr);
}

void tst_QAudioRecorder::testDevices()
{
//    audiosource = new QMediaRecorder;
//    QList<QAudioDeviceInfo> devices = mockIntegration->audioInputs();
//    QVERIFY(devices.size() > 0);
//    QVERIFY(devices.at(0).id() == "device1");
//    QVERIFY(audiosource->audioInputDescription("device1").compare("dev1 comment") == 0);
//    QVERIFY(audiosource->defaultAudioInput() == "device1");
//    QVERIFY(audiosource->isAvailable() == true);

//    QSignalSpy checkSignal(audiosource, SIGNAL(audioInputChanged(QString)));
//    audiosource->setAudioInput("device2");
//    QVERIFY(audiosource->audioInput().compare("device2") == 0);
//    QVERIFY(checkSignal.count() == 1);
//    QVERIFY(audiosource->isAvailable() == true);
}

void tst_QAudioRecorder::testAvailability()
{
    QMediaRecorder source;

    QVERIFY(source.isAvailable());
}

QTEST_GUILESS_MAIN(tst_QAudioRecorder)

#include "tst_qaudiorecorder.moc"
