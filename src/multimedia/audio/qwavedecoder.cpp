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

#include "qwavedecoder.h"

#include <QtCore/qtimer.h>
#include <QtCore/qendian.h>
#include <limits.h>
#include <qdebug.h>

QT_BEGIN_NAMESPACE

namespace  {

void bswap2(char *data, qsizetype count) noexcept
{
    for (qsizetype i = 0; i < count; ++i) {
        qSwap(data[0], data[1]);
        ++count;
        data += 2;
    }
}

void bswap3(char *data, qsizetype count) noexcept
{
    for (qsizetype i = 0; i < count; ++i) {
        qSwap(data[0], data[2]);
        ++count;
        data += 3;
    }
}

void bswap4(char *data, qsizetype count) noexcept
{
    for (qsizetype i = 0; i < count; ++i) {
        qSwap(data[0], data[3]);
        qSwap(data[1], data[2]);
        ++count;
        data += 4;
    }
}

}

QWaveDecoder::QWaveDecoder(QIODevice *device, QObject *parent)
    : QIODevice(parent),
      device(device)
{
}

QWaveDecoder::QWaveDecoder(QIODevice *device, const QAudioFormat &format, QObject *parent)
    : QIODevice(parent),
        device(device),
        format(format)
{
}

QWaveDecoder::~QWaveDecoder() = default;

bool QWaveDecoder::open(QIODevice::OpenMode mode)
{
    bool canOpen = false;
    if (mode & QIODevice::ReadOnly && mode & ~QIODevice::WriteOnly) {
        canOpen = QIODevice::open(mode | QIODevice::Unbuffered);
        if (canOpen && enoughDataAvailable())
            handleData();
        else
            connect(device, SIGNAL(readyRead()), SLOT(handleData()));
        return canOpen;
    }

    if (mode & QIODevice::WriteOnly) {
        if (format.sampleFormat() != QAudioFormat::Int16)
            return false; // data format is not supported
        canOpen = QIODevice::open(mode);
        if (canOpen && writeHeader())
            haveHeader = true;
        return canOpen;
    }
    return QIODevice::open(mode);
}

void QWaveDecoder::close()
{
    if (isOpen() && (openMode() & QIODevice::WriteOnly)) {
        Q_ASSERT(dataSize < INT_MAX);
        if (device->isOpen())
            Q_ASSERT(writeDataLength());
        else
            qWarning() << "Failed to finalize output because output device was closed";
    }
    QIODevice::close();
}

bool QWaveDecoder::seek(qint64 pos)
{
    return device->seek(pos);
}

qint64 QWaveDecoder::pos() const
{
    return device->pos();
}

QAudioFormat QWaveDecoder::audioFormat() const
{
    return format;
}

QIODevice* QWaveDecoder::getDevice()
{
    return device;
}

int QWaveDecoder::duration() const
{
    if (openMode() & QIODevice::WriteOnly)
        return 0;
    return dataSize * 1000 / (format.bytesPerFrame() * format.sampleRate());
}

qint64 QWaveDecoder::size() const
{
    if (openMode() & QIODevice::ReadOnly)
        return haveFormat ? dataSize : 0;
    else
        return device->size();
}

bool QWaveDecoder::isSequential() const
{
    return device->isSequential();
}

qint64 QWaveDecoder::bytesAvailable() const
{
    return haveFormat ? device->bytesAvailable() : 0;
}

qint64 QWaveDecoder::headerLength()
{
    return HeaderLength;
}

qint64 QWaveDecoder::readData(char *data, qint64 maxlen)
{
    if (!haveFormat)
        return 0;

    qint64 nSamples = maxlen / format.bytesPerSample();
    maxlen = nSamples * format.bytesPerFrame();
    device->read(data, maxlen);

    if (!byteSwap || format.bytesPerFrame() == 1)
        return maxlen;

    switch (format.bytesPerSample()) {
    case 2:
        bswap2(data, nSamples);
        break;
    case 3:
        bswap3(data, nSamples);
        break;
    case 4:
        bswap4(data, nSamples);
        break;
    }
    return maxlen;

}

qint64 QWaveDecoder::writeData(const char *data, qint64 len)
{
    if (!haveHeader)
        return 0;
    qint64 written = device->write(data, len);
    dataSize += written;
    return written;
}

bool QWaveDecoder::writeHeader()
{
    if (device->size() != 0)
        return false;

#ifndef Q_LITTLE_ENDIAN
    // only implemented for LITTLE ENDIAN
    return false;
#endif

    CombinedHeader header;

    memset(&header, 0, HeaderLength);

    // RIFF header
    memcpy(header.riff.descriptor.id,"RIFF",4);
    qToLittleEndian<quint32>(quint32(dataSize + HeaderLength - 8),
                             reinterpret_cast<unsigned char*>(&header.riff.descriptor.size));
    memcpy(header.riff.type, "WAVE",4);

    // WAVE header
    memcpy(header.wave.descriptor.id,"fmt ",4);
    qToLittleEndian<quint32>(quint32(16),
                             reinterpret_cast<unsigned char*>(&header.wave.descriptor.size));
    qToLittleEndian<quint16>(quint16(1),
                             reinterpret_cast<unsigned char*>(&header.wave.audioFormat));
    qToLittleEndian<quint16>(quint16(format.channelCount()),
                             reinterpret_cast<unsigned char*>(&header.wave.numChannels));
    qToLittleEndian<quint32>(quint32(format.sampleRate()),
                             reinterpret_cast<unsigned char*>(&header.wave.sampleRate));
    qToLittleEndian<quint32>(quint32(format.sampleRate() * format.bytesPerFrame()),
                             reinterpret_cast<unsigned char*>(&header.wave.byteRate));
    qToLittleEndian<quint16>(quint16(format.channelCount() * format.bytesPerSample()),
                             reinterpret_cast<unsigned char*>(&header.wave.blockAlign));
    qToLittleEndian<quint16>(quint16(format.bytesPerSample() * 8),
                             reinterpret_cast<unsigned char*>(&header.wave.bitsPerSample));

    // DATA header
    memcpy(header.data.descriptor.id,"data",4);
    qToLittleEndian<quint32>(quint32(dataSize),
                             reinterpret_cast<unsigned char*>(&header.data.descriptor.size));

    return device->write(reinterpret_cast<const char *>(&header), HeaderLength);
}

bool QWaveDecoder::writeDataLength()
{
#ifndef Q_LITTLE_ENDIAN
    // only implemented for LITTLE ENDIAN
    return false;
#endif

    if (isSequential())
        return false;

    // seek to RIFF header size, see header.riff.descriptor.size above
    if (!device->seek(4))
        return false;

    quint32 length = dataSize + HeaderLength - 8;
    if (device->write(reinterpret_cast<const char *>(&length), 4) != 4)
        return false;

    // seek to DATA header size, see header.data.descriptor.size above
    if (!device->seek(40))
        return false;

    return device->write(reinterpret_cast<const char *>(&dataSize), 4);
}

void QWaveDecoder::parsingFailed()
{
    Q_ASSERT(device);
    device->disconnect(SIGNAL(readyRead()), this, SLOT(handleData()));
    emit parsingError();
}

void QWaveDecoder::handleData()
{
    if (openMode() == QIODevice::WriteOnly)
        return;

    // As a special "state", if we have junk to skip, we do
    if (junkToSkip > 0) {
        discardBytes(junkToSkip); // this also updates junkToSkip

        // If we couldn't skip all the junk, return
        if (junkToSkip > 0) {
            // We might have run out
            if (device->atEnd())
                parsingFailed();
            return;
        }
    }

    if (state == QWaveDecoder::InitialState) {
        if (device->bytesAvailable() < qint64(sizeof(RIFFHeader)))
            return;

        RIFFHeader riff;
        device->read(reinterpret_cast<char *>(&riff), sizeof(RIFFHeader));

        // RIFF = little endian RIFF, RIFX = big endian RIFF
        if (((qstrncmp(riff.descriptor.id, "RIFF", 4) != 0) && (qstrncmp(riff.descriptor.id, "RIFX", 4) != 0))
                || qstrncmp(riff.type, "WAVE", 4) != 0) {
            parsingFailed();
            return;
        }

        state = QWaveDecoder::WaitingForFormatState;
        bigEndian = (qstrncmp(riff.descriptor.id, "RIFX", 4) == 0);
        byteSwap = (bigEndian != (QSysInfo::ByteOrder == QSysInfo::BigEndian));
    }

    if (state == QWaveDecoder::WaitingForFormatState) {
        if (findChunk("fmt ")) {
            chunk descriptor;
            peekChunk(&descriptor);

            quint32 rawChunkSize = descriptor.size + sizeof(chunk);
            if (device->bytesAvailable() < qint64(rawChunkSize))
                return;

            WAVEHeader wave;
            device->read(reinterpret_cast<char *>(&wave), sizeof(WAVEHeader));

            if (rawChunkSize > sizeof(WAVEHeader))
                discardBytes(rawChunkSize - sizeof(WAVEHeader));

            // Swizzle this
            if (bigEndian) {
                wave.audioFormat = qFromBigEndian<quint16>(wave.audioFormat);
            } else {
                wave.audioFormat = qFromLittleEndian<quint16>(wave.audioFormat);
            }

            if (wave.audioFormat != 0 && wave.audioFormat != 1) {
                // 32bit wave files have format == 0xFFFE (WAVE_FORMAT_EXTENSIBLE).
                // but don't support them at the moment.
                parsingFailed();
                return;
            }

            int bps;
            int rate;
            int channels;
            if (bigEndian) {
                 bps = qFromBigEndian<quint16>(wave.bitsPerSample);
                 rate = qFromBigEndian<quint32>(wave.sampleRate);
                 channels = qFromBigEndian<quint16>(wave.numChannels);
            } else {
                bps = qFromLittleEndian<quint16>(wave.bitsPerSample);
                rate = qFromLittleEndian<quint32>(wave.sampleRate);
                channels = qFromLittleEndian<quint16>(wave.numChannels);
            }

            QAudioFormat::SampleFormat fmt = QAudioFormat::Unknown;
            switch(bps) {
            case 8:
                fmt = QAudioFormat::UInt8;
                break;
            case 16:
                fmt = QAudioFormat::Int16;
                break;
            case 24:
                fmt = QAudioFormat::Unknown;
                break;
            case 32:
                fmt = QAudioFormat::Int32;
                break;
            }
            if (fmt == QAudioFormat::Unknown || rate == 0 || channels == 0) {
                parsingFailed();
                return;
            }

            format.setSampleFormat(fmt);
            format.setSampleRate(/*qFromBigEndian<quint32>*/(wave.sampleRate));
            format.setChannelCount(/*qFromBigEndian<quint16>*/(wave.numChannels));

            state = QWaveDecoder::WaitingForDataState;
        }
    }

    if (state == QWaveDecoder::WaitingForDataState) {
        if (findChunk("data")) {
            device->disconnect(SIGNAL(readyRead()), this, SLOT(handleData()));

            chunk descriptor;
            device->read(reinterpret_cast<char *>(&descriptor), sizeof(chunk));
            if (bigEndian)
                descriptor.size = qFromBigEndian<quint32>(descriptor.size);
            else
                descriptor.size = qFromLittleEndian<quint32>(descriptor.size);

            dataSize = descriptor.size; //means the data size from the data header, not the actual file size

            haveFormat = true;
            connect(device, SIGNAL(readyRead()), SIGNAL(readyRead()));
            emit formatKnown();

            return;
        }
    }

    // If we hit the end without finding data, it's a parsing error
    if (device->atEnd()) {
        parsingFailed();
    }
}

bool QWaveDecoder::enoughDataAvailable()
{
    chunk descriptor;
    if (!peekChunk(&descriptor, false))
        return false;

    // This is only called for the RIFF/RIFX header, before bigEndian is set,
    // so we have to manually swizzle
    if (qstrncmp(descriptor.id, "RIFX", 4) == 0)
        descriptor.size = qFromBigEndian<quint32>(descriptor.size);
    if (qstrncmp(descriptor.id, "RIFF", 4) == 0)
        descriptor.size = qFromLittleEndian<quint32>(descriptor.size);

    if (device->bytesAvailable() < qint64(sizeof(chunk) + descriptor.size))
        return false;

    return true;
}

bool QWaveDecoder::findChunk(const char *chunkId)
{
    chunk descriptor;

    do {
        if (!peekChunk(&descriptor))
            return false;

        if (qstrncmp(descriptor.id, chunkId, 4) == 0)
            return true;

        // It's possible that bytes->available() is less than the chunk size
        // if it's corrupt.
        junkToSkip = qint64(sizeof(chunk) + descriptor.size);

        // Skip the current amount
        if (junkToSkip > 0)
            discardBytes(junkToSkip);

        // If we still have stuff left, just exit and try again later
        // since we can't call peekChunk
        if (junkToSkip > 0)
            return false;

    } while (device->bytesAvailable() > 0);

    return false;
}

bool QWaveDecoder::peekChunk(chunk *pChunk, bool handleEndianness)
{
    if (device->bytesAvailable() < qint64(sizeof(chunk)))
        return false;

    if (!device->peek(reinterpret_cast<char *>(pChunk), sizeof(chunk)))
        return false;

    if (handleEndianness) {
        if (bigEndian)
            pChunk->size = qFromBigEndian<quint32>(pChunk->size);
        else
            pChunk->size = qFromLittleEndian<quint32>(pChunk->size);
    }
    return true;
}

void QWaveDecoder::discardBytes(qint64 numBytes)
{
    // Discards a number of bytes
    // If the iodevice doesn't have this many bytes in it,
    // remember how much more junk we have to skip.
    if (device->isSequential()) {
        QByteArray r = device->read(qMin(numBytes, qint64(16384))); // uggh, wasted memory, limit to a max of 16k
        if (r.size() < numBytes)
            junkToSkip = numBytes - r.size();
        else
            junkToSkip = 0;
    } else {
        quint64 origPos = device->pos();
        device->seek(device->pos() + numBytes);
        junkToSkip = origPos + numBytes - device->pos();
    }
}

QT_END_NAMESPACE

#include "moc_qwavedecoder.cpp"
