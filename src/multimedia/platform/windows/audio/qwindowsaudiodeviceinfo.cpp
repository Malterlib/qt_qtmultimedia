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
// INTERNAL USE ONLY: Do NOT use for any other purpose.
//


#include <QtCore/qt_windows.h>
#include <QtCore/QDataStream>
#include <QtCore/QIODevice>
#include <mmsystem.h>
#include "qwindowsaudiodeviceinfo_p.h"
#include "qwindowsaudioutils_p.h"


QWindowsAudioDeviceInfo::QWindowsAudioDeviceInfo(QByteArray dev, int waveID, const QString &description, QAudio::Mode mode)
    : QAudioDeviceInfoPrivate(dev, mode),
      devId(waveID)
{
    this->description = description;
    preferredFormat.setSampleRate(44100);
    preferredFormat.setChannelCount(2);
    preferredFormat.setSampleFormat(QAudioFormat::Int16);

    DWORD fmt = 0;

    if(mode == QAudio::AudioOutput) {
        WAVEOUTCAPS woc;
        if (waveOutGetDevCaps(devId, &woc, sizeof(WAVEOUTCAPS)) == MMSYSERR_NOERROR)
            fmt = woc.dwFormats;
    } else {
        WAVEINCAPS woc;
        if (waveInGetDevCaps(devId, &woc, sizeof(WAVEINCAPS)) == MMSYSERR_NOERROR)
            fmt = woc.dwFormats;
    }

    if (!fmt)
        return;

    // Check sample size
    if ((fmt & WAVE_FORMAT_1M08)
        || (fmt & WAVE_FORMAT_1S08)
        || (fmt & WAVE_FORMAT_2M08)
        || (fmt & WAVE_FORMAT_2S08)
        || (fmt & WAVE_FORMAT_4M08)
        || (fmt & WAVE_FORMAT_4S08)
        || (fmt & WAVE_FORMAT_48M08)
        || (fmt & WAVE_FORMAT_48S08)
        || (fmt & WAVE_FORMAT_96M08)
        || (fmt & WAVE_FORMAT_96S08)) {
        supportedSampleFormats.append(QAudioFormat::UInt8);
    }
    if ((fmt & WAVE_FORMAT_1M16)
        || (fmt & WAVE_FORMAT_1S16)
        || (fmt & WAVE_FORMAT_2M16)
        || (fmt & WAVE_FORMAT_2S16)
        || (fmt & WAVE_FORMAT_4M16)
        || (fmt & WAVE_FORMAT_4S16)
        || (fmt & WAVE_FORMAT_48M16)
        || (fmt & WAVE_FORMAT_48S16)
        || (fmt & WAVE_FORMAT_96M16)
        || (fmt & WAVE_FORMAT_96S16)) {
        supportedSampleFormats.append(QAudioFormat::Int16);
    }

    supportedSampleRates = { INT_MAX, 0 };
    // Check sample rate
    if ((fmt & WAVE_FORMAT_1M08)
       || (fmt & WAVE_FORMAT_1S08)
       || (fmt & WAVE_FORMAT_1M16)
       || (fmt & WAVE_FORMAT_1S16)) {
        supportedSampleRates.minimum = qMin(supportedSampleRates.minimum, 11025);
        supportedSampleRates.maximum = qMin(supportedSampleRates.maximum, 11025);
    }
    if ((fmt & WAVE_FORMAT_2M08)
       || (fmt & WAVE_FORMAT_2S08)
       || (fmt & WAVE_FORMAT_2M16)
       || (fmt & WAVE_FORMAT_2S16)) {
        supportedSampleRates.minimum = qMin(supportedSampleRates.minimum, 22050);
        supportedSampleRates.maximum = qMin(supportedSampleRates.maximum, 22050);
    }
    if ((fmt & WAVE_FORMAT_4M08)
       || (fmt & WAVE_FORMAT_4S08)
       || (fmt & WAVE_FORMAT_4M16)
       || (fmt & WAVE_FORMAT_4S16)) {
        supportedSampleRates.minimum = qMin(supportedSampleRates.minimum, 44100);
        supportedSampleRates.maximum = qMin(supportedSampleRates.maximum, 44100);
    }
    if ((fmt & WAVE_FORMAT_48M08)
        || (fmt & WAVE_FORMAT_48S08)
        || (fmt & WAVE_FORMAT_48M16)
        || (fmt & WAVE_FORMAT_48S16)) {
        supportedSampleRates.minimum = qMin(supportedSampleRates.minimum, 48000);
        supportedSampleRates.maximum = qMin(supportedSampleRates.maximum, 48000);
    }
    if ((fmt & WAVE_FORMAT_96M08)
       || (fmt & WAVE_FORMAT_96S08)
       || (fmt & WAVE_FORMAT_96M16)
       || (fmt & WAVE_FORMAT_96S16)) {
        supportedSampleRates.minimum = qMin(supportedSampleRates.minimum, 96000);
        supportedSampleRates.maximum = qMin(supportedSampleRates.maximum, 96000);
    }

    // Check channel count
    if (fmt & WAVE_FORMAT_1M08
            || fmt & WAVE_FORMAT_1M16
            || fmt & WAVE_FORMAT_2M08
            || fmt & WAVE_FORMAT_2M16
            || fmt & WAVE_FORMAT_4M08
            || fmt & WAVE_FORMAT_4M16
            || fmt & WAVE_FORMAT_48M08
            || fmt & WAVE_FORMAT_48M16
            || fmt & WAVE_FORMAT_96M08
            || fmt & WAVE_FORMAT_96M16) {
        supportedChannelCounts.minimum = 1;
        supportedChannelCounts.maximum = 1;
    }
    if (fmt & WAVE_FORMAT_1S08
            || fmt & WAVE_FORMAT_1S16
            || fmt & WAVE_FORMAT_2S08
            || fmt & WAVE_FORMAT_2S16
            || fmt & WAVE_FORMAT_4S08
            || fmt & WAVE_FORMAT_4S16
            || fmt & WAVE_FORMAT_48S08
            || fmt & WAVE_FORMAT_48S16
            || fmt & WAVE_FORMAT_96S08
            || fmt & WAVE_FORMAT_96S16) {
        supportedChannelCounts.minimum = qMin(supportedChannelCounts.minimum, 96000);
        supportedChannelCounts.maximum = qMin(supportedChannelCounts.maximum, 96000);
    }

    // WAVEOUTCAPS and WAVEINCAPS contains information only for the previously tested parameters.
    // WaveOut and WaveInt might actually support more formats, the only way to know is to try
    // opening the device with it.
    QAudioFormat testFormat;
    testFormat.setChannelCount(supportedChannelCounts.maximum);
    testFormat.setSampleRate(supportedSampleRates.maximum);
    const QAudioFormat defaultTestFormat(testFormat);

    // Check if float samples are supported
    testFormat.setSampleFormat(QAudioFormat::Float);
    if (testSettings(testFormat))
        supportedSampleFormats.append(QAudioFormat::Float);

    // Check channel counts > 2
    testFormat = defaultTestFormat;
    for (int i = 18; i > 2; --i) { // <mmreg.h> defines 18 different channels
        testFormat.setChannelCount(i);
        if (testSettings(testFormat)) {
            supportedChannelCounts.maximum = i;
            break;
        }
    }
}

QWindowsAudioDeviceInfo::~QWindowsAudioDeviceInfo()
{
}

bool QWindowsAudioDeviceInfo::testSettings(const QAudioFormat& format) const
{
    WAVEFORMATEXTENSIBLE wfx;
    if (qt_convertFormat(format, &wfx)) {
        // query only, do not open device
        if (mode == QAudio::AudioOutput) {
            return (waveOutOpen(NULL, UINT_PTR(devId), &wfx.Format, 0, 0,
                                WAVE_FORMAT_QUERY) == MMSYSERR_NOERROR);
        } else { // AudioInput
            return (waveInOpen(NULL, UINT_PTR(devId), &wfx.Format, 0, 0,
                                WAVE_FORMAT_QUERY) == MMSYSERR_NOERROR);
        }
    }

    return false;
}

QT_END_NAMESPACE
