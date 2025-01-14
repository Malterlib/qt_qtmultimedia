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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef QAUDIOINPUTPULSE_H
#define QAUDIOINPUTPULSE_H

#include <QtCore/qfile.h>
#include <QtCore/qtimer.h>
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qelapsedtimer.h>
#include <QtCore/qiodevice.h>
#include <QtCore/private/qringbuffer_p.h>

#include "qaudio.h"
#include "qaudiodeviceinfo.h"
#include <private/qaudiosystem_p.h>

#include <private/qgstutils_p.h>
#include <gst/app/gstappsink.h>

QT_BEGIN_NAMESPACE

class GStreamerInputPrivate;

class QGStreamerAudioInput
    : public QAbstractAudioInput
{
    Q_OBJECT
    friend class GStreamerInputPrivate;
public:
    QGStreamerAudioInput(const QByteArray &device);
    ~QGStreamerAudioInput();

    void start(QIODevice *device) override;
    QIODevice *start() override;
    void stop() override;
    void reset() override;
    void suspend() override;
    void resume() override;
    int bytesReady() const override;
    int periodSize() const override;
    void setBufferSize(int value) override;
    int bufferSize() const override;
    void setNotifyInterval(int milliSeconds) override;
    int notifyInterval() const override;
    qint64 processedUSecs() const override;
    qint64 elapsedUSecs() const override;
    QAudio::Error error() const override;
    QAudio::State state() const override;
    void setFormat(const QAudioFormat &format) override;
    QAudioFormat format() const override;

    void setVolume(qreal volume) override;
    qreal volume() const override;

private:
    void setState(QAudio::State state);
    void setError(QAudio::Error error);

    QGstElement createAppSink();
    static GstFlowReturn new_sample(GstAppSink *, gpointer user_data);
    static void eos(GstAppSink *, gpointer user_data);

    bool open();
    void close();

    static gboolean busMessage(GstBus *bus, GstMessage *msg, gpointer user_data);

    qint64 m_totalTimeValue = 0;
    QIODevice *m_audioSink = nullptr;
    QAudioFormat m_format;
    QAudio::Error m_errorState = QAudio::NoError;
    QAudio::State m_deviceState = QAudio::StoppedState;
    qreal m_volume = 1.;

    QRingBuffer m_buffer;
    bool m_pullMode = true;
    bool m_opened = false;
    int m_bytesAvailable = 0;
    int m_bufferSize = 0;
    int m_periodSize = 0;
    int m_intervalTime = 1000;
    qint64 m_elapsedTimeOffset = 0;
    QElapsedTimer m_timeStamp;
    QElapsedTimer m_clockStamp;
    QByteArray m_device;
    QByteArray m_tempBuffer;

    QGstElement gstInput;
    QGstPipeline gstPipeline;
    QGstElement gstVolume;
    QGstElement gstAppSink;
};

class GStreamerInputPrivate : public QIODevice
{
    Q_OBJECT
public:
    GStreamerInputPrivate(QGStreamerAudioInput *audio);
    ~GStreamerInputPrivate() {};

    qint64 readData(char *data, qint64 len) override;
    qint64 writeData(const char *data, qint64 len) override;
    qint64 bytesAvailable() const override;
    bool isSequential() const override { return true; }
private:
    QGStreamerAudioInput *m_audioDevice;
};

QT_END_NAMESPACE

#endif
