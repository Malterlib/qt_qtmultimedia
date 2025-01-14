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

#include <QtQml/qqmlextensionplugin.h>
#include <QtQml/qqml.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcomponent.h>
#include "qsoundeffect.h"

#include <private/qdeclarativevideooutput_p.h>
#include "qabstractvideofilter.h"

#include "qdeclarativemultimediaglobal_p.h"
#include "qdeclarativemediametadata_p.h"
#include "qdeclarativeaudio_p.h"
#include "qdeclarativeplaylist_p.h"
#include "qdeclarativecamera_p.h"
#include "qdeclarativecamerapreviewprovider_p.h"
#include "qdeclarativecameraexposure_p.h"
#include "qdeclarativecameraflash_p.h"
#include "qdeclarativecamerafocus_p.h"
#include "qdeclarativecameraimageprocessing_p.h"
#include "qdeclarativetorch_p.h"
#include <QAbstractVideoSurface>

QML_DECLARE_TYPE(QSoundEffect)

QT_BEGIN_NAMESPACE

static QObject *multimedia_global_object(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
{
    Q_UNUSED(qmlEngine);
    return new QDeclarativeMultimediaGlobal(jsEngine);
}

class QMultimediaDeclarativeModule : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
    QMultimediaDeclarativeModule(QObject *parent = nullptr) : QQmlExtensionPlugin(parent) { }
    void registerTypes(const char *uri) override
    {
        Q_ASSERT(QLatin1String(uri) == QLatin1String("QtMultimedia"));

        // 5.0 types
        qmlRegisterType<QSoundEffect>(uri, 5, 0, "SoundEffect");
        qmlRegisterType<QDeclarativeAudio>(uri, 5, 0, "Audio");
        qmlRegisterType<QDeclarativeAudio>(uri, 5, 0, "MediaPlayer");
        qmlRegisterType<QDeclarativeVideoOutput>(uri, 5, 0, "VideoOutput");
        qmlRegisterType<QDeclarativeCamera>(uri, 5, 0, "Camera");
        qmlRegisterUncreatableType<QDeclarativeCameraCapture>(uri, 5, 0, "CameraCapture",
                                tr("CameraCapture is provided by Camera"));
        qmlRegisterUncreatableType<QDeclarativeCameraRecorder>(uri, 5, 0, "CameraRecorder",
                                tr("CameraRecorder is provided by Camera"));
        qmlRegisterUncreatableType<QDeclarativeCameraExposure>(uri, 5, 0, "CameraExposure",
                                tr("CameraExposure is provided by Camera"));
        qmlRegisterUncreatableType<QDeclarativeCameraFocus>(uri, 5, 0, "CameraFocus",
                                tr("CameraFocus is provided by Camera"));
        qmlRegisterUncreatableType<QDeclarativeCameraFlash>(uri, 5, 0, "CameraFlash",
                                tr("CameraFlash is provided by Camera"));
        qmlRegisterUncreatableType<QDeclarativeTorch>(uri, 5, 0, "CameraTorch",
                                tr("CameraTorch is provided by Camera"));
        qmlRegisterUncreatableType<QDeclarativeCameraImageProcessing>(uri, 5, 0, "CameraImageProcessing",
                                tr("CameraImageProcessing is provided by Camera"));

        // 5.2 types
        qmlRegisterType<QDeclarativeVideoOutput, 2>(uri, 5, 2, "VideoOutput");

        // 5.3 types
        // Nothing changed, but adding "import QtMultimedia 5.3" in QML will fail unless at
        // least one type is registered for that version.
        qmlRegisterType<QSoundEffect>(uri, 5, 3, "SoundEffect");

        // 5.4 types
        qmlRegisterSingletonType<QDeclarativeMultimediaGlobal>(uri, 5, 4, "QtMultimedia", multimedia_global_object);
        qmlRegisterType<QDeclarativeCamera, 1>(uri, 5, 4, "Camera");

        // 5.5 types
        qmlRegisterUncreatableType<QDeclarativeCameraImageProcessing, 1>(uri, 5, 5, "CameraImageProcessing", tr("CameraImageProcessing is provided by Camera"));
        qmlRegisterType<QDeclarativeCamera, 2>(uri, 5, 5, "Camera");

        // 5.6 types
        qmlRegisterType<QDeclarativeAudio, 1>(uri, 5, 6, "Audio");
        qmlRegisterType<QDeclarativeAudio, 1>(uri, 5, 6, "MediaPlayer");
        qmlRegisterType<QDeclarativePlaylist>(uri, 5, 6, "Playlist");
        qmlRegisterType<QDeclarativePlaylistItem>(uri, 5, 6, "PlaylistItem");

        // 5.7 types
        qmlRegisterType<QDeclarativePlaylist, 1>(uri, 5, 7, "Playlist");
        qmlRegisterUncreatableType<QDeclarativeCameraImageProcessing, 2>(uri, 5, 7, "CameraImageProcessing",
                                tr("CameraImageProcessing is provided by Camera"));

        // 5.8 types (nothing new, re-register one of the types)
        qmlRegisterType<QSoundEffect>(uri, 5, 8, "SoundEffect");

        // 5.9 types
        qmlRegisterType<QDeclarativeAudio, 2>(uri, 5, 9, "Audio");
        qmlRegisterType<QDeclarativeAudio, 2>(uri, 5, 9, "MediaPlayer");
        qmlRegisterUncreatableType<QDeclarativeCameraCapture, 1>(uri, 5, 9, "CameraCapture",
                                tr("CameraCapture is provided by Camera"));
        qmlRegisterUncreatableType<QDeclarativeCameraFlash, 1>(uri, 5, 9, "CameraFlash",
                                tr("CameraFlash is provided by Camera"));

        // 5.11 types
        qmlRegisterType<QDeclarativeAudio, 3>(uri, 5, 11, "Audio");
        qmlRegisterType<QDeclarativeAudio, 3>(uri, 5, 11, "MediaPlayer");
        qmlRegisterUncreatableType<QDeclarativeCameraFocus, 1>(uri, 5, 11, "CameraFocus",
                                tr("CameraFocus is provided by Camera"));
        qmlRegisterUncreatableType<QDeclarativeCameraExposure, 1>(uri, 5, 11, "CameraExposure",
                                tr("CameraExposure is provided by Camera"));
        qmlRegisterUncreatableType<QDeclarativeCameraImageProcessing, 3>(uri, 5, 11, "CameraImageProcessing",
                                tr("CameraImageProcessing is provided by Camera"));

        qmlRegisterAnonymousType<QDeclarativeMediaMetaData>(uri, 5);
        qmlRegisterAnonymousType<QAbstractVideoFilter>(uri, 5);

        // 5.13 types
        qmlRegisterType<QDeclarativeVideoOutput, 13>(uri, 5, 13, "VideoOutput");

        // 5.15 types
        qmlRegisterType<QDeclarativeAudio, 15>(uri, 5, 15, "MediaPlayer");
        qmlRegisterType<QDeclarativeVideoOutput, 15>(uri, 5, 15, "VideoOutput");
        qmlRegisterAnonymousType<QAbstractVideoSurface>(uri, 5);

        // The minor version used to be the current Qt 5 minor. For compatibility it is the last
        // Qt 5 release.
        qmlRegisterModule(uri, 5, 15);
    }

    void initializeEngine(QQmlEngine *engine, const char *uri) override
    {
        Q_UNUSED(uri);
        engine->addImageProvider("camera", new QDeclarativeCameraPreviewProvider);
    }
};

QT_END_NAMESPACE

#include "multimedia.moc"

