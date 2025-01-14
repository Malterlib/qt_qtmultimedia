/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Mobility Components.
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

/* Media related snippets */
#include <QFile>
#include <QTimer>
#include <QBuffer>

#include "qmediaplaylist.h"
#include "qmediarecorder.h"
#include "qplatformmediaplayer_p.h"
#include "qmediaplayer.h"
#include "qvideowidget.h"
#include "qcameraimagecapture.h"
#include "qcamera.h"
#include "qcameraviewfinder.h"
#include "qaudiorecorder.h"
#include <QAbstractVideoSurface>

class MediaExample : public QObject {
    Q_OBJECT

    void MediaControl();
    void MediaPlayer();
    void MediaRecorder();
    void AudioRecorder();
    void EncoderSettings();
    void ImageEncoderSettings();

private:
    // Common naming
    QVideoWidget *videoWidget;
    QWidget *widget;
    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QMediaContent video;
    QMediaRecorder *recorder;
    QCamera *camera;
    QCameraViewfinder *viewfinder;
    QCameraImageCapture *imageCapture;
    QString fileName;
    QAudioRecorder *audioRecorder;

    QMediaContent image1;
    QMediaContent image2;
    QMediaContent image3;
};

void MediaExample::MediaControl()
{
}


void MediaExample::EncoderSettings()
{
    //! [Audio encoder settings]
    QAudioEncoderSettings audioSettings;
    audioSettings.setCodec("audio/mpeg");
    audioSettings.setChannelCount(2);

    recorder->setAudioSettings(audioSettings);
    //! [Audio encoder settings]

    //! [Video encoder settings]
    QVideoEncoderSettings videoSettings;
    videoSettings.setCodec("video/mpeg2");
    videoSettings.setResolution(640, 480);

    recorder->setVideoSettings(videoSettings);
    //! [Video encoder settings]
}

void MediaExample::ImageEncoderSettings()
{
    //! [Image encoder settings]
    QImageEncoderSettings imageSettings;
    imageSettings.setCodec("image/jpeg");
    imageSettings.setResolution(1600, 1200);

    imageCapture->setEncodingSettings(imageSettings);
    //! [Image encoder settings]
}

void MediaExample::MediaPlayer()
{
    //! [Player]
    player = new QMediaPlayer;
    connect(player, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
    player->setMedia(QUrl::fromLocalFile("/Users/me/Music/coolsong.mp3"));
    player->setVolume(50);
    player->play();
    //! [Player]

    //! [Local playback]
    player = new QMediaPlayer;
    // ...
    player->setMedia(QUrl::fromLocalFile("/Users/me/Music/coolsong.mp3"));
    player->setVolume(50);
    player->play();
    //! [Local playback]

    //! [Audio playlist]
    player = new QMediaPlayer;

    playlist = new QMediaPlaylist(player);
    playlist->addMedia(QUrl("http://example.com/myfile1.mp3"));
    playlist->addMedia(QUrl("http://example.com/myfile2.mp3"));
    // ...
    playlist->setCurrentIndex(1);
    player->play();
    //! [Audio playlist]

    //! [Movie playlist]
    playlist = new QMediaPlaylist;
    playlist->addMedia(QUrl("http://example.com/movie1.mp4"));
    playlist->addMedia(QUrl("http://example.com/movie2.mp4"));
    playlist->addMedia(QUrl("http://example.com/movie3.mp4"));
    playlist->setCurrentIndex(1);

    player = new QMediaPlayer;
    player->setPlaylist(playlist);

    videoWidget = new QVideoWidget;
    player->setVideoOutput(videoWidget);
    videoWidget->show();

    player->play();
    //! [Movie playlist]

    //! [Pipeline]
    player = new QMediaPlayer;
    player->setMedia(QUrl("gst-pipeline: videotestsrc ! autovideosink"));
    player->play();
    //! [Pipeline]

    //! [Pipeline Surface]
    class Surface : public QAbstractVideoSurface
    {
    public:
        Surface(QObject *p) : QAbstractVideoSurface(p) { }
        QList<QVideoFrame::PixelFormat> supportedPixelFormats(QVideoFrame::HandleType) const override
        {
            // Make sure that the driver supports this pixel format.
            return QList<QVideoFrame::PixelFormat>() << QVideoFrame::Format_YUYV;
        }

        // Video frames are handled here.
        bool present(const QVideoFrame &) override { return true; }
    };

    player = new QMediaPlayer;
    player->setVideoOutput(new Surface(player));
    player->setMedia(QUrl("gst-pipeline: videotestsrc ! qtvideosink"));
    player->play();
    //! [Pipeline Surface]

    //! [Pipeline Widget]
    player = new QMediaPlayer;
    videoWidget = new QVideoWidget;
    videoWidget->show();
    player->setVideoOutput(videoWidget);
    player->setMedia(QUrl("gst-pipeline: videotestsrc ! xvimagesink name=\"qtvideosink\""));
    player->play();
    //! [Pipeline Widget]

    //! [Pipeline appsrc]
    QImage img("images/qt-logo.png");
    img = img.convertToFormat(QImage::Format_ARGB32);
    QByteArray ba(reinterpret_cast<const char *>(img.bits()), img.sizeInBytes());
    QBuffer buffer(&ba);
    buffer.open(QIODevice::ReadOnly);
    player = new QMediaPlayer;
    player->setMedia(QUrl("gst-pipeline: appsrc blocksize=4294967295 ! \
        video/x-raw,format=BGRx,framerate=30/1,width=200,height=147 ! \
        coloreffects preset=heat ! videoconvert ! video/x-raw,format=I420 ! jpegenc ! rtpjpegpay ! \
        udpsink host=127.0.0.1 port=5000"), &buffer);
    player->play();

    QMediaPlayer *receiver = new QMediaPlayer;
    videoWidget = new QVideoWidget;
    receiver->setVideoOutput(videoWidget);
    receiver->setMedia(QUrl("gst-pipeline: udpsrc port=5000 ! \
        application/x-rtp,encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! \
        xvimagesink name=qtvideosink"));
    receiver->play();
    // Content will be shown in this widget.
    videoWidget->show();
    //! [Pipeline appsrc]
}

void MediaExample::MediaRecorder()
{
    //! [Media recorder]
    recorder = new QMediaRecorder(camera);

    QMediaEncoderSettings audioSettings;
    audioSettings.setFormat(QMediaEncoderSettings::MP3);
    audioSettings.setQuality(QMediaEncoderSettings::HighQuality);

    recorder->setAudioSettings(audioSettings);

    recorder->setOutputLocation(QUrl::fromLocalFile(fileName));
    recorder->record();
    //! [Media recorder]
}

void MediaExample::AudioRecorder()
{
    //! [Audio recorder]
    audioRecorder = new QAudioRecorder;

    QMediaEncoderSettings audioSettings;
    audioSettings.setFormat(QMediaEncoderSettings::MP3);
    audioSettings.setQuality(QMediaEncoderSettings::HighQuality);

    audioRecorder->setEncodingSettings(audioSettings);

    audioRecorder->setOutputLocation(QUrl::fromLocalFile("test.amr"));
    audioRecorder->record();
    //! [Audio recorder]

    //! [Audio recorder inputs]
    const QStringList inputs = audioRecorder->audioInputs();
    QString selectedInput = audioRecorder->defaultAudioInput();

    for (const QString &input : inputs) {
        QString description = audioRecorder->audioInputDescription(input);
        // show descriptions to user and allow selection
        selectedInput = input;
    }

    audioRecorder->setAudioInput(selectedInput);
    //! [Audio recorder inputs]
}

