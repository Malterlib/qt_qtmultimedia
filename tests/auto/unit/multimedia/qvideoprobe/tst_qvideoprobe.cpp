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

#include <qvideoprobe.h>
#include <qmediarecorder.h>
#include <qmediaplayer.h>

//TESTED_COMPONENT=src/multimedia

#include "mockmediarecorderservice.h"
#include "mockmediaplayerservice.h"
#include "mockmediasource.h"
#include "qmockintegration_p.h"

QT_USE_NAMESPACE

class tst_QVideoProbe: public QObject
{
    Q_OBJECT

public slots:
    void init();
    void cleanup();

private slots:
    void testNullService();
    void testPlayer();
    void testPlayerDeleteRecorder();
    void testPlayerDeleteProbe();
    void testRecorder();

private:
    QMediaPlayer *player;
    QMockIntegration *mockIntegration = nullptr;
};

void tst_QVideoProbe::init()
{
    mockIntegration = new QMockIntegration;
    player = nullptr;
}

void tst_QVideoProbe::cleanup()
{
    delete player;
    delete mockIntegration;
}

void tst_QVideoProbe::testNullService()
{
    mockIntegration->setFlags(QMockIntegration::NoPlayerInterface);
    player = new QMediaPlayer;

    QVERIFY(!player->isAvailable());
    QCOMPARE(player->availability(), QMultimedia::ServiceMissing);

    QVideoProbe probe;
    QVERIFY(!probe.isActive());
    QVERIFY(!probe.setSource(player));
    QVERIFY(!probe.isActive());
    delete player;
    player = nullptr;
    QVERIFY(!probe.isActive());
}

void tst_QVideoProbe::testPlayer()
{
    player = new QMediaPlayer;
    QVERIFY(player->isAvailable());

    QVideoProbe probe;
    QVERIFY(!probe.isActive());
    QVERIFY(probe.setSource(player));
    QVERIFY(probe.isActive());
    probe.setSource((QMediaPlayer*)nullptr);
    QVERIFY(!probe.isActive());
}

void tst_QVideoProbe::testPlayerDeleteRecorder()
{
    player = new QMediaPlayer;
    QVERIFY(player->isAvailable());

    QVideoProbe probe;
    QVERIFY(!probe.isActive());
    QVERIFY(probe.setSource(player));
    QVERIFY(probe.isActive());

    delete player;
    player = nullptr;
    QVERIFY(!probe.isActive());
    probe.setSource((QMediaPlayer*)nullptr);
    QVERIFY(!probe.isActive());
}

void tst_QVideoProbe::testPlayerDeleteProbe()
{
    player = new QMediaPlayer;
    QVERIFY(player->isAvailable());

    QVideoProbe *probe = new QVideoProbe;
    QVERIFY(!probe->isActive());
    QVERIFY(probe->setSource(player));
    QVERIFY(probe->isActive());

    delete probe;
    QVERIFY(player->isAvailable());
}

void tst_QVideoProbe::testRecorder()
{
    QMediaRecorder recorder;
    QVERIFY(recorder.isAvailable());

    QVideoProbe probe;
    QVERIFY(!probe.isActive());
    QVERIFY(!probe.setSource(&recorder)); // No QMediaVideoProbeControl available
    QVERIFY(!probe.isActive());
}

QTEST_GUILESS_MAIN(tst_QVideoProbe)

#include "tst_qvideoprobe.moc"