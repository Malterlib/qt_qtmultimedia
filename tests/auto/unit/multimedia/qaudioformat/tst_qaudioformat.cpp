/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
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
#include <QtCore/qlocale.h>
#include <qaudioformat.h>

#include <QStringList>
#include <QList>

//TESTED_COMPONENT=src/multimedia

class tst_QAudioFormat : public QObject
{
    Q_OBJECT

public:
    tst_QAudioFormat(QObject* parent=nullptr) : QObject(parent) {}

private slots:
    void checkNull();
    void checkSampleFormat();
    void checkEquality();
    void checkAssignment();
    void checkSampleRate();
    void checkChannelCount();

    void checkSizes();
    void checkSizes_data();
};

void tst_QAudioFormat::checkNull()
{
    // Default constructed QAudioFormat is invalid.
    QAudioFormat audioFormat0;
    QVERIFY(!audioFormat0.isValid());

    // validity is transferred
    QAudioFormat audioFormat1(audioFormat0);
    QVERIFY(!audioFormat1.isValid());

    audioFormat0.setSampleRate(44100);
    audioFormat0.setChannelCount(2);
    audioFormat0.setSampleFormat(QAudioFormat::Int16);
    QVERIFY(audioFormat0.isValid());
}

void tst_QAudioFormat::checkSampleFormat()
{
    QAudioFormat audioFormat;
    audioFormat.setSampleFormat(QAudioFormat::Int16);
    QVERIFY(audioFormat.sampleFormat() == QAudioFormat::Int16);
    QTest::ignoreMessage(QtDebugMsg, "Int16");
    qDebug() << QAudioFormat::Int16;

    audioFormat.setSampleFormat(QAudioFormat::Unknown);
    QVERIFY(audioFormat.sampleFormat() == QAudioFormat::Unknown);
    QTest::ignoreMessage(QtDebugMsg, "Unknown");
    qDebug() << QAudioFormat::Unknown;

    audioFormat.setSampleFormat(QAudioFormat::UInt8);
    QVERIFY(audioFormat.sampleFormat() == QAudioFormat::UInt8);
    QTest::ignoreMessage(QtDebugMsg, "UInt8");
    qDebug() << QAudioFormat::UInt8;

    audioFormat.setSampleFormat(QAudioFormat::Float);
    QVERIFY(audioFormat.sampleFormat() == QAudioFormat::Float);
    QTest::ignoreMessage(QtDebugMsg, "Float");
    qDebug() << QAudioFormat::Float;
}

void tst_QAudioFormat::checkEquality()
{
    QAudioFormat audioFormat0;
    QAudioFormat audioFormat1;

    // Null formats are equivalent
    QVERIFY(audioFormat0 == audioFormat1);
    QVERIFY(!(audioFormat0 != audioFormat1));

    // on filled formats
    audioFormat0.setSampleRate(8000);
    audioFormat0.setChannelCount(1);
    audioFormat0.setSampleFormat(QAudioFormat::UInt8);

    audioFormat1.setSampleRate(8000);
    audioFormat1.setChannelCount(1);
    audioFormat1.setSampleFormat(QAudioFormat::UInt8);

    QVERIFY(audioFormat0 == audioFormat1);
    QVERIFY(!(audioFormat0 != audioFormat1));

    audioFormat0.setSampleRate(44100);
    QVERIFY(audioFormat0 != audioFormat1);
    QVERIFY(!(audioFormat0 == audioFormat1));
}

void tst_QAudioFormat::checkAssignment()
{
    QAudioFormat audioFormat0;
    QAudioFormat audioFormat1;

    audioFormat0.setSampleRate(8000);
    audioFormat0.setChannelCount(1);
    audioFormat0.setSampleFormat(QAudioFormat::UInt8);

    audioFormat1 = audioFormat0;
    QVERIFY(audioFormat1 == audioFormat0);

    QAudioFormat audioFormat2(audioFormat0);
    QVERIFY(audioFormat2 == audioFormat0);
}

/* sampleRate() API property test. */
void tst_QAudioFormat::checkSampleRate()
{
    QAudioFormat audioFormat;
    QVERIFY(audioFormat.sampleRate() == -1);

    audioFormat.setSampleRate(123);
    QVERIFY(audioFormat.sampleRate() == 123);
}

/* channelCount() API property test. */
void tst_QAudioFormat::checkChannelCount()
{
    // channels is the old name for channelCount, so
    // they should always be equal
    QAudioFormat audioFormat;
    QVERIFY(audioFormat.channelCount() == -1);
    QVERIFY(audioFormat.channelCount() == -1);

    audioFormat.setChannelCount(123);
    QVERIFY(audioFormat.channelCount() == 123);
    QVERIFY(audioFormat.channelCount() == 123);

    audioFormat.setChannelCount(5);
    QVERIFY(audioFormat.channelCount() == 5);
    QVERIFY(audioFormat.channelCount() == 5);
}

void tst_QAudioFormat::checkSizes()
{
    QFETCH(QAudioFormat, format);
    QFETCH(int, frameSize);
    QFETCH(int, byteCount);
    QFETCH(int, frameCount);
    QFETCH(qint64, durationForByte);
    QFETCH(int, byteForFrame);
    QFETCH(qint64, durationForByteForDuration);
    QFETCH(int, byteForDuration);
    QFETCH(int, framesForDuration);

    QCOMPARE(format.bytesPerFrame(), frameSize);

    // Byte input
    QCOMPARE(format.framesForBytes(byteCount), frameCount);
    QCOMPARE(format.durationForBytes(byteCount), durationForByte);

    // Framecount input
    QCOMPARE(format.bytesForFrames(frameCount), byteForFrame);
    QCOMPARE(format.durationForFrames(frameCount), durationForByte);

    // Duration input
    QCOMPARE(format.bytesForDuration(durationForByteForDuration), byteForDuration);
    QCOMPARE(format.framesForDuration(durationForByte), frameCount);
    QCOMPARE(format.framesForDuration(durationForByteForDuration), framesForDuration);
}

void tst_QAudioFormat::checkSizes_data()
{
    QTest::addColumn<QAudioFormat>("format");
    QTest::addColumn<int>("frameSize");
    QTest::addColumn<int>("byteCount");
    QTest::addColumn<qint64>("durationForByte");
    QTest::addColumn<int>("frameCount"); // output of sampleCountforByte, input for byteForFrame
    QTest::addColumn<int>("byteForFrame");
    QTest::addColumn<qint64>("durationForByteForDuration"); // input for byteForDuration
    QTest::addColumn<int>("byteForDuration");
    QTest::addColumn<int>("framesForDuration");

    QAudioFormat f;
    QTest::newRow("invalid") << f << 0 << 0 << 0LL << 0 << 0 << 0LL << 0 << 0;

    f.setChannelCount(1);
    f.setSampleRate(8000);
    f.setSampleFormat(QAudioFormat::UInt8);

    qint64 qrtr = 250000LL;
    qint64 half = 500000LL;
    qint64 one = 1000000LL;
    qint64 two = 2000000LL;

    // No rounding errors with mono 8 bit
    QTest::newRow("1ch_8b_8k_signed_4000") << f << 1 << 4000 << half << 4000 << 4000 << half << 4000 << 4000;
    QTest::newRow("1ch_8b_8k_signed_8000") << f << 1 << 8000 << one << 8000 << 8000 << one << 8000 << 8000;
    QTest::newRow("1ch_8b_8k_signed_16000") << f << 1 << 16000 << two << 16000 << 16000 << two << 16000 << 16000;

    // Mono 16bit
    f.setSampleFormat(QAudioFormat::Int16);
    QTest::newRow("1ch_16b_8k_signed_8000") << f << 2 << 8000 << half << 4000 << 8000 << half << 8000 << 4000;
    QTest::newRow("1ch_16b_8k_signed_16000") << f << 2 << 16000 << one << 8000 << 16000 << one << 16000 << 8000;

    // Rounding errors
    QTest::newRow("1ch_16b_8k_signed_8001") << f << 2 << 8001 << half << 4000 << 8000 << half << 8000 << 4000;
    QTest::newRow("1ch_16b_8k_signed_8000_duration1") << f << 2 << 8000 << half << 4000 << 8000 << half + 1 << 8000 << 4000;
    QTest::newRow("1ch_16b_8k_signed_8000_duration2") << f << 2 << 8000 << half << 4000 << 8000 << half + 124 << 8000 << 4000;
    QTest::newRow("1ch_16b_8k_signed_8000_duration3") << f << 2 << 8000 << half << 4000 << 8000 << half + 125 << 8002 << 4001;
    QTest::newRow("1ch_16b_8k_signed_8000_duration4") << f << 2 << 8000 << half << 4000 << 8000 << half + 126 << 8002 << 4001;

    // Stereo 16 bit
    f.setChannelCount(2);
    QTest::newRow("2ch_16b_8k_signed_8000") << f << 4 << 8000 << qrtr << 2000 << 8000 << qrtr << 8000 << 2000;
    QTest::newRow("2ch_16b_8k_signed_16000") << f << 4 << 16000 << half << 4000 << 16000 << half << 16000 << 4000;

    // More rounding errors
    // First rounding bytes
    QTest::newRow("2ch_16b_8k_signed_8001") << f << 4 << 8001 << qrtr << 2000 << 8000 << qrtr << 8000 << 2000;
    QTest::newRow("2ch_16b_8k_signed_8002") << f << 4 << 8002 << qrtr << 2000 << 8000 << qrtr << 8000 << 2000;
    QTest::newRow("2ch_16b_8k_signed_8003") << f << 4 << 8003 << qrtr << 2000 << 8000 << qrtr << 8000 << 2000;

    // Then rounding duration
    // 8khz = 125us per frame
    QTest::newRow("2ch_16b_8k_signed_8000_duration1") << f << 4 << 8000 << qrtr << 2000 << 8000 << qrtr + 1 << 8000 << 2000;
    QTest::newRow("2ch_16b_8k_signed_8000_duration2") << f << 4 << 8000 << qrtr << 2000 << 8000 << qrtr + 124 << 8000 << 2000;
    QTest::newRow("2ch_16b_8k_signed_8000_duration3") << f << 4 << 8000 << qrtr << 2000 << 8000 << qrtr + 125 << 8004 << 2001;
    QTest::newRow("2ch_16b_8k_signed_8000_duration4") << f << 4 << 8000 << qrtr << 2000 << 8000 << qrtr + 126 << 8004 << 2001;
}

QTEST_MAIN(tst_QAudioFormat)

#include "tst_qaudioformat.moc"
