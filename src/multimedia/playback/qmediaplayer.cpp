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

#include "qmediaplayer_p.h"
#include "qvideosurfaces_p.h"

#include <private/qplatformmediaintegration_p.h>

#include <QtCore/qcoreevent.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>
#include <QtCore/qpointer.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qtemporaryfile.h>
#include <QtCore/qdir.h>

QT_BEGIN_NAMESPACE

/*!
    \class QMediaPlayer
    \brief The QMediaPlayer class allows the playing of a media source.
    \inmodule QtMultimedia
    \ingroup multimedia
    \ingroup multimedia_playback

    The QMediaPlayer class is a high level media playback class. It can be used
    to playback such content as songs, movies and internet radio. The content
    to playback is specified as a QUrl object, which can be thought of as a
    main or canonical URL with additional information attached. When provided
    with a QUrl playback may be able to commence.

    \snippet multimedia-snippets/media.cpp Player

    QVideoWidget can be used with QMediaPlayer for video rendering.

    \sa QVideoWidget
*/

void QMediaPlayerPrivate::_q_notify()
{
    Q_Q(QMediaPlayer);

    const QMetaObject* m = q->metaObject();

    // QTBUG-57045
    // we create a copy of notifyProperties container to ensure that if a property is removed
    // from the original container as a result of invoking propertyChanged signal, the iterator
    // won't become invalidated
    QSet<int> properties = notifyProperties;

    for (int pi : qAsConst(properties)) {
        QMetaProperty p = m->property(pi);
        p.notifySignal().invoke(
            q, QGenericArgument(p.metaType().name(), p.read(q).data()));
    }
}

void QMediaPlayerPrivate::setState(QMediaPlayer::State ps)
{
    Q_Q(QMediaPlayer);

    if (ps != state) {
        state = ps;

        if (ps == QMediaPlayer::PlayingState)
            q->addPropertyWatch("position");
        else
            q->removePropertyWatch("position");

        emit q->stateChanged(ps);
    }
}

void QMediaPlayerPrivate::setStatus(QMediaPlayer::MediaStatus s)
{
    Q_Q(QMediaPlayer);

    if (int(s) == ignoreNextStatusChange) {
        ignoreNextStatusChange = -1;
        return;
    }

    if (s != status) {
        status = s;

        switch (s) {
        case QMediaPlayer::StalledMedia:
        case QMediaPlayer::BufferingMedia:
            q->addPropertyWatch("bufferStatus");
            break;
        default:
            q->removePropertyWatch("bufferStatus");
            break;
        }

        emit q->mediaStatusChanged(s);
    }
}

void QMediaPlayerPrivate::setError(int error, const QString &errorString)
{
    Q_Q(QMediaPlayer);

    this->error = QMediaPlayer::Error(error);
    this->errorString = errorString;
    emit q->error(this->error);
}

void QMediaPlayerPrivate::setMedia(const QUrl &media, QIODevice *stream)
{
    Q_Q(QMediaPlayer);

    if (!control)
        return;

    QScopedPointer<QFile> file;

    // Backends can't play qrc files directly.
    // If the backend supports StreamPlayback, we pass a QFile for that resource.
    // If it doesn't, we copy the data to a temporary file and pass its path.
    if (!media.isEmpty() && !stream && media.scheme() == QLatin1String("qrc")) {
        qrcMedia = media;

        file.reset(new QFile(QLatin1Char(':') + media.path()));
        if (!file->open(QFile::ReadOnly)) {
            setError(QMediaPlayer::ResourceError, QMediaPlayer::tr("Attempting to play invalid Qt resource"));
            QMetaObject::invokeMethod(q, "_q_mediaStatusChanged", Qt::QueuedConnection,
                                      Q_ARG(QMediaPlayer::MediaStatus, QMediaPlayer::InvalidMedia));
            file.reset();
            // Ignore the next NoMedia status change, we just want to clear the current media
            // on the backend side since we can't load the new one and we want to be in the
            // InvalidMedia status.
            ignoreNextStatusChange = QMediaPlayer::NoMedia;
            control->setMedia(QUrl(), nullptr);

        } else if (hasStreamPlaybackFeature) {
            control->setMedia(media, file.data());
        } else {
#if QT_CONFIG(temporaryfile)
#if defined(Q_OS_ANDROID)
            QString tempFileName = QDir::tempPath() + media.path();
            QDir().mkpath(QFileInfo(tempFileName).path());
            QTemporaryFile *tempFile = QTemporaryFile::createNativeFile(*file);
            if (!tempFile->rename(tempFileName))
                qWarning() << "Could not rename temporary file to:" << tempFileName;
#else
            QTemporaryFile *tempFile = new QTemporaryFile;

            // Preserve original file extension, some backends might not load the file if it doesn't
            // have an extension.
            const QString suffix = QFileInfo(*file).suffix();
            if (!suffix.isEmpty())
                tempFile->setFileTemplate(tempFile->fileTemplate() + QLatin1Char('.') + suffix);

            // Copy the qrc data into the temporary file
            tempFile->open();
            char buffer[4096];
            while (true) {
                qint64 len = file->read(buffer, sizeof(buffer));
                if (len < 1)
                    break;
                tempFile->write(buffer, len);
            }
            tempFile->close();
#endif
            file.reset(tempFile);
            control->setMedia(QUrl(QUrl::fromLocalFile(file->fileName())), nullptr);
#else
            qWarning("Qt was built with -no-feature-temporaryfile: playback from resource file is not supported!");
#endif
        }
    } else {
        qrcMedia = QUrl();
        control->setMedia(media, stream);
    }

    qrcFile.swap(file); // Cleans up any previous file
}

QList<QMediaMetaData> QMediaPlayerPrivate::trackMetaData(QPlatformMediaPlayer::TrackType s) const
{
    QList<QMediaMetaData> tracks;
    if (control) {
        int count = control->trackCount(s);
        for (int i = 0; i < count; ++i) {
            tracks.append(control->trackMetaData(s, i));
        }
    }
    return tracks;
}

/*!
    Construct a QMediaPlayer instance
    parented to \a parent and with \a flags.
*/

QMediaPlayer::QMediaPlayer(QObject *parent)
    : QObject(*new QMediaPlayerPrivate, parent)
{
    Q_D(QMediaPlayer);

    d->notifyTimer = new QTimer(this);
    d->notifyTimer->setInterval(1000);
    connect(d->notifyTimer, SIGNAL(timeout()), SLOT(_q_notify()));

    d->control = QPlatformMediaIntegration::instance()->createPlayer(this);
    if (!d->control) { // ### Should this be an assertion?
        d->setError(QMediaPlayer::ResourceError, QMediaPlayer::tr("Platform does not support media playback."));
        return;
    }
    Q_ASSERT(d->control);

    d->state = d->control->state();
    d->status = d->control->mediaStatus();

    if (d->state == PlayingState)
        addPropertyWatch("position");

    if (d->status == StalledMedia || d->status == BufferingMedia)
        addPropertyWatch("bufferStatus");

    d->hasStreamPlaybackFeature = d->control->streamPlaybackSupported();
}


/*!
    Destroys the player object.
*/

QMediaPlayer::~QMediaPlayer()
{
    Q_D(QMediaPlayer);

    // Disconnect everything to prevent notifying
    // when a receiver is already destroyed.
    disconnect();

    delete d->control;
}

int QMediaPlayer::notifyInterval() const
{
    return d_func()->notifyTimer->interval();
}

void QMediaPlayer::setNotifyInterval(int milliSeconds)
{
    Q_D(QMediaPlayer);

    if (d->notifyTimer->interval() != milliSeconds) {
        d->notifyTimer->setInterval(milliSeconds);

        emit notifyIntervalChanged(milliSeconds);
    }
}

/*!
    Watch the property \a name. The property's notify signal will be emitted
    once every \c notifyInterval milliseconds.

    \sa notifyInterval
*/

void QMediaPlayer::addPropertyWatch(QByteArray const &name)
{
    Q_D(QMediaPlayer);

    const QMetaObject* m = metaObject();

    int index = m->indexOfProperty(name.constData());

    if (index != -1 && m->property(index).hasNotifySignal()) {
        d->notifyProperties.insert(index);

        if (!d->notifyTimer->isActive())
            d->notifyTimer->start();
    }
}

/*!
    Remove property \a name from the list of properties whose changes are
    regularly signaled.

    \sa notifyInterval
*/

void QMediaPlayer::removePropertyWatch(QByteArray const &name)
{
    Q_D(QMediaPlayer);

    int index = metaObject()->indexOfProperty(name.constData());

    if (index != -1) {
        d->notifyProperties.remove(index);

        if (d->notifyProperties.isEmpty())
            d->notifyTimer->stop();
    }
}

QUrl QMediaPlayer::media() const
{
    Q_D(const QMediaPlayer);

    return d->rootMedia;
}

/*!
    Returns the stream source of media data.

    This is only valid if a stream was passed to setMedia().

    \sa setMedia()
*/

const QIODevice *QMediaPlayer::mediaStream() const
{
    Q_D(const QMediaPlayer);

    // When playing a resource file, we might have passed a QFile to the backend. Hide it from
    // the user.
    if (d->control && d->qrcMedia.isEmpty())
        return d->control->mediaStream();

    return nullptr;
}

QMediaPlayer::State QMediaPlayer::state() const
{
    Q_D(const QMediaPlayer);

    // In case if EndOfMedia status is already received
    // but state is not.
    if (d->control != nullptr
        && d->status == QMediaPlayer::EndOfMedia
        && d->state != d->control->state()) {
        return d->control->state();
    }

    return d->state;
}

QMediaPlayer::MediaStatus QMediaPlayer::mediaStatus() const
{
    return d_func()->status;
}

qint64 QMediaPlayer::duration() const
{
    Q_D(const QMediaPlayer);

    if (d->control != nullptr)
        return d->control->duration();

    return -1;
}

qint64 QMediaPlayer::position() const
{
    Q_D(const QMediaPlayer);

    if (d->control != nullptr)
        return d->control->position();

    return 0;
}

int QMediaPlayer::volume() const
{
    Q_D(const QMediaPlayer);

    if (d->control != nullptr)
        return d->control->volume();

    return 0;
}

bool QMediaPlayer::isMuted() const
{
    Q_D(const QMediaPlayer);

    if (d->control != nullptr)
        return d->control->isMuted();

    return false;
}

int QMediaPlayer::bufferStatus() const
{
    Q_D(const QMediaPlayer);

    if (d->control != nullptr)
        return d->control->bufferStatus();

    return 0;
}

bool QMediaPlayer::isAudioAvailable() const
{
    Q_D(const QMediaPlayer);

    if (d->control != nullptr)
        return d->control->isAudioAvailable();

    return false;
}

bool QMediaPlayer::isVideoAvailable() const
{
    Q_D(const QMediaPlayer);

    if (d->control != nullptr)
        return d->control->isVideoAvailable();

    return false;
}

bool QMediaPlayer::isSeekable() const
{
    Q_D(const QMediaPlayer);

    if (d->control != nullptr)
        return d->control->isSeekable();

    return false;
}

qreal QMediaPlayer::playbackRate() const
{
    Q_D(const QMediaPlayer);

    if (d->control != nullptr)
        return d->control->playbackRate();

    return 0.0;
}

/*!
    Returns the current error state.
*/

QMediaPlayer::Error QMediaPlayer::error() const
{
    return d_func()->error;
}

QString QMediaPlayer::errorString() const
{
    return d_func()->errorString;
}

/*!
    Start or resume playing the current source.
*/

void QMediaPlayer::play()
{
    Q_D(QMediaPlayer);

    if (!d->control)
        return;

    // Reset error conditions
    d->error = NoError;
    d->errorString = QString();

    d->control->play();
}

/*!
    Pause playing the current source.
*/

void QMediaPlayer::pause()
{
    Q_D(QMediaPlayer);

    if (d->control != nullptr)
        d->control->pause();
}

/*!
    Stop playing, and reset the play position to the beginning.
*/

void QMediaPlayer::stop()
{
    Q_D(QMediaPlayer);

    if (d->control != nullptr)
        d->control->stop();
}

void QMediaPlayer::setPosition(qint64 position)
{
    Q_D(QMediaPlayer);

    if (d->control == nullptr)
        return;

    d->control->setPosition(qMax(position, 0ll));
}

void QMediaPlayer::setVolume(int v)
{
    Q_D(QMediaPlayer);

    if (d->control == nullptr)
        return;

    int clamped = qBound(0, v, 100);
    if (clamped == volume())
        return;

    d->control->setVolume(clamped);
}

void QMediaPlayer::setMuted(bool muted)
{
    Q_D(QMediaPlayer);

    if (d->control == nullptr || muted == isMuted())
        return;

    d->control->setMuted(muted);
}

void QMediaPlayer::setPlaybackRate(qreal rate)
{
    Q_D(QMediaPlayer);

    if (d->control != nullptr)
        d->control->setPlaybackRate(rate);
}

/*!
    Sets the current \a media source.

    If a \a stream is supplied; media data will be read from it instead of resolving the media
    source. In this case the url should be provided to resolve additional information
    about the media such as mime type. The \a stream must be open and readable.
    For macOS the \a stream should be also seekable.

    Setting the media to a null QUrl will cause the player to discard all
    information relating to the current media source and to cease all I/O operations related
    to that media.

    \note This function returns immediately after recording the specified source of the media.
    It does not wait for the media to finish loading and does not check for errors. Listen for
    the mediaStatusChanged() and error() signals to be notified when the media is loaded and
    when an error occurs during loading.

    Since Qt 5.12.2, the url scheme \c gst-pipeline provides custom pipelines
    for the GStreamer backend.

    \snippet multimedia-snippets/media.cpp Pipeline

    If QAbstractVideoSurface is used as the video output,
    \c qtvideosink can be used as a video sink element directly in the pipeline.
    After that the surface will receive the video frames in QAbstractVideoSurface::present().

    \snippet multimedia-snippets/media.cpp Pipeline Surface

    If QVideoWidget is used as the video output
    and the pipeline contains a video sink element named \c qtvideosink,
    current QVideoWidget will be used to render the video.

    \snippet multimedia-snippets/media.cpp Pipeline Widget

    If the pipeline contains appsrc element, it will be used to push data from \a stream.

    \snippet multimedia-snippets/media.cpp Pipeline appsrc
*/

void QMediaPlayer::setMedia(const QUrl &media, QIODevice *stream)
{
    Q_D(QMediaPlayer);
    stop();

    QUrl oldMedia = d->rootMedia;
    d->rootMedia = media;

    if (oldMedia != media) {
        d->setMedia(media, stream);
        emit mediaChanged(d->rootMedia);
    }
}

/*!
    Sets the audio output to \a device.

    Setting a null QAudioDeviceInfo, sets the output to the system default.

    Returns true if the output could be changed, false otherwise.
 */
bool QMediaPlayer::setAudioOutput(const QAudioDeviceInfo &device)
{
    Q_D(QMediaPlayer);
    return d->control->setAudioOutput(device);
}

QAudioDeviceInfo QMediaPlayer::audioOutput() const
{
    Q_D(const QMediaPlayer);
    return d->control->audioOutput();
}

QList<QMediaMetaData> QMediaPlayer::audioTracks() const
{
    Q_D(const QMediaPlayer);
    return d->trackMetaData(QPlatformMediaPlayer::AudioStream);
}

QList<QMediaMetaData> QMediaPlayer::videoTracks() const
{
    Q_D(const QMediaPlayer);
    return d->trackMetaData(QPlatformMediaPlayer::VideoStream);
}

QList<QMediaMetaData> QMediaPlayer::subtitleTracks() const
{
    Q_D(const QMediaPlayer);
    return d->trackMetaData(QPlatformMediaPlayer::SubtitleStream);
}

int QMediaPlayer::activeAudioTrack() const
{
    Q_D(const QMediaPlayer);
    if (d->control)
        return d->control->activeTrack(QPlatformMediaPlayer::AudioStream);
    return 0;
}

int QMediaPlayer::activeVideoTrack() const
{
    Q_D(const QMediaPlayer);
    if (d->control)
        return d->control->activeTrack(QPlatformMediaPlayer::VideoStream);
    return 0;
}

int QMediaPlayer::activeSubtitleTrack() const
{
    Q_D(const QMediaPlayer);
    if (d->control)
        return d->control->activeTrack(QPlatformMediaPlayer::SubtitleStream);
    return 0;
}

void QMediaPlayer::setActiveAudioTrack(int index)
{
    Q_D(QMediaPlayer);
    if (d->control)
        d->control->setActiveTrack(QPlatformMediaPlayer::AudioStream, index);
}

void QMediaPlayer::setActiveVideoTrack(int index)
{
    Q_D(QMediaPlayer);
    if (d->control)
        d->control->setActiveTrack(QPlatformMediaPlayer::VideoStream, index);
}

void QMediaPlayer::setActiveSubtitleTrack(int index)
{
    Q_D(QMediaPlayer);
    if (d->control)
        d->control->setActiveTrack(QPlatformMediaPlayer::SubtitleStream, index);
}

/*!
    Attach a video \a output to the media player.

    If the media player has already video output attached,
    it will be replaced with a new one.
*/
void QMediaPlayer::setVideoOutput(QObject *output)
{
    auto *mo = output->metaObject();
    QAbstractVideoSurface *surface = nullptr;
    if (output && !mo->invokeMethod(output, "videoSurface", Q_RETURN_ARG(QAbstractVideoSurface *, surface))) {
        qWarning() << "QMediaPlayer::setVideoOutput: Object" << output->metaObject()->className() << "does not have a videoSurface()";
        return;
    }
    setVideoOutput(surface);
}

/*!
    Sets a video \a surface as the video output of a media player.

    If a video output has already been set on the media player the new surface
    will replace it.
*/

void QMediaPlayer::setVideoOutput(QAbstractVideoSurface *surface)
{
    Q_D(QMediaPlayer);

    if (!d->control)
        return;

    d->control->setVideoSurface(surface);
}

/*!
    \since 5.15
    Sets multiple video surfaces as the video output of a media player.
    This allows the media player to render video frames on different surfaces.

    All video surfaces must support at least one shared \c QVideoFrame::PixelFormat.

    If a video output has already been set on the media player the new surfaces
    will replace it.

    \sa QAbstractVideoSurface::supportedPixelFormats
*/

void QMediaPlayer::setVideoOutput(const QList<QAbstractVideoSurface *> &surfaces)
{
    setVideoOutput(!surfaces.empty() ? new QVideoSurfaces(surfaces, this) : nullptr);
}

/*! \reimp */
bool QMediaPlayer::isAvailable() const
{
    Q_D(const QMediaPlayer);

    if (!d->control)
        return false;

    return true;
}

QMediaMetaData QMediaPlayer::metaData() const
{
    Q_D(const QMediaPlayer);
    return d->control->metaData();
}

QAudio::Role QMediaPlayer::audioRole() const
{
    Q_D(const QMediaPlayer);
    return d->audioRole;
}

void QMediaPlayer::setAudioRole(QAudio::Role audioRole)
{
    Q_D(QMediaPlayer);
    if (d->audioRole == audioRole)
        return;

    d->audioRole = audioRole;
    d->control->setAudioRole(audioRole);
    if (!d->customAudioRole.isEmpty()) {
        d->customAudioRole.clear();
        emit customAudioRoleChanged(QString());
    }
    emit audioRoleChanged(audioRole);

}

/*!
    Returns a list of supported audio roles.

    If setting the audio role is not supported, an empty list is returned.

    \since 5.6
    \sa audioRole
*/
QList<QAudio::Role> QMediaPlayer::supportedAudioRoles() const
{
    Q_D(const QMediaPlayer);

    return d->control->supportedAudioRoles();
}

QString QMediaPlayer::customAudioRole() const
{
    Q_D(const QMediaPlayer);

    return d->customAudioRole;
}

void QMediaPlayer::setCustomAudioRole(const QString &audioRole)
{
    Q_D(QMediaPlayer);
    if (d->audioRole == QAudio::CustomRole && d->customAudioRole == audioRole)
        return;

    d->customAudioRole = audioRole;
    d->control->setCustomAudioRole(audioRole);
    if (d->audioRole != QAudio::CustomRole) {
        d->audioRole = QAudio::CustomRole;
        emit audioRoleChanged(QAudio::CustomRole);
    }
    emit customAudioRoleChanged(audioRole);
}

/*!
    Returns a list of supported custom audio roles. An empty list may
    indicate that the supported custom audio roles aren't known. The
    list may not be complete.

    \since 5.11
    \sa customAudioRole
*/
QStringList QMediaPlayer::supportedCustomAudioRoles() const
{
    Q_D(const QMediaPlayer);
    return d->control->supportedCustomAudioRoles();
}

// Enums
/*!
    \enum QMediaPlayer::State

    Defines the current state of a media player.

    \value StoppedState The media player is not playing content, playback will begin from the start
    of the current track.
    \value PlayingState The media player is currently playing content.
    \value PausedState The media player has paused playback, playback of the current track will
    resume from the position the player was paused at.
*/

/*!
    \enum QMediaPlayer::MediaStatus

    Defines the status of a media player's current media.

    \value UnknownMediaStatus The status of the media cannot be determined.
    \value NoMedia The is no current media.  The player is in the StoppedState.
    \value LoadingMedia The current media is being loaded. The player may be in any state.
    \value LoadedMedia The current media has been loaded. The player is in the StoppedState.
    \value StalledMedia Playback of the current media has stalled due to insufficient buffering or
    some other temporary interruption.  The player is in the PlayingState or PausedState.
    \value BufferingMedia The player is buffering data but has enough data buffered for playback to
    continue for the immediate future.  The player is in the PlayingState or PausedState.
    \value BufferedMedia The player has fully buffered the current media.  The player is in the
    PlayingState or PausedState.
    \value EndOfMedia Playback has reached the end of the current media.  The player is in the
    StoppedState.
    \value InvalidMedia The current media cannot be played.  The player is in the StoppedState.
*/

/*!
    \enum QMediaPlayer::Error

    Defines a media player error condition.

    \value NoError No error has occurred.
    \value ResourceError A media resource couldn't be resolved.
    \value FormatError The format of a media resource isn't (fully) supported.  Playback may still
    be possible, but without an audio or video component.
    \value NetworkError A network error occurred.
    \value AccessDeniedError There are not the appropriate permissions to play a media resource.
    \value ServiceMissingError A valid playback service was not found, playback cannot proceed.
*/

// Signals
/*!
    \fn QMediaPlayer::error(QMediaPlayer::Error error)

    Signals that an \a error condition has occurred.

    \sa errorString()
*/

/*!
    \fn void QMediaPlayer::stateChanged(State state)

    Signal the \a state of the Player object has changed.
*/

/*!
    \fn QMediaPlayer::mediaStatusChanged(QMediaPlayer::MediaStatus status)

    Signals that the \a status of the current media has changed.

    \sa mediaStatus()
*/

/*!
    \fn void QMediaPlayer::mediaChanged(const QUrl &media);

    Signals that the media source has been changed to \a media.

    \sa media(), currentMediaChanged()
*/

/*!
    \fn void QMediaPlayer::currentMediaChanged(const QUrl &media);

    Signals that the current playing content has been changed to \a media.

    \sa currentMedia(), mediaChanged()
*/

/*!
    \fn void QMediaPlayer::playbackRateChanged(qreal rate);

    Signals the playbackRate has changed to \a rate.
*/

/*!
    \fn void QMediaPlayer::seekableChanged(bool seekable);

    Signals the \a seekable status of the player object has changed.
*/

/*!
    \fn void QMediaPlayer::audioRoleChanged(QAudio::Role role)

    Signals that the audio \a role of the media player has changed.

    \since 5.6
*/

/*!
    \fn void QMediaPlayer::customAudioRoleChanged(const QString &role)

    Signals that the audio \a role of the media player has changed.

    \since 5.11
*/

// Properties
/*!
    \property QMediaPlayer::state
    \brief the media player's playback state.

    By default this property is QMediaPlayer::Stopped

    \sa mediaStatus(), play(), pause(), stop()
*/

/*!
    \property QMediaPlayer::error
    \brief a string describing the last error condition.

    \sa error()
*/

/*!
    \property QMediaPlayer::media
    \brief the active media source being used by the player object.

    The player object will use the QUrl for selection of the content to
    be played.

    By default this property has a null QUrl.

    Setting this property to a null QUrl will cause the player to discard all
    information relating to the current media source and to cease all I/O operations related
    to that media.

    \sa QUrl
*/

/*!
    \property QMediaPlayer::mediaStatus
    \brief the status of the current media stream.

    The stream status describes how the playback of the current stream is
    progressing.

    By default this property is QMediaPlayer::NoMedia

    \sa state
*/

/*!
    \property QMediaPlayer::duration
    \brief the duration of the current media.

    The value is the total playback time in milliseconds of the current media.
    The value may change across the life time of the QMediaPlayer object and
    may not be available when initial playback begins, connect to the
    durationChanged() signal to receive status notifications.
*/

/*!
    \property QMediaPlayer::position
    \brief the playback position of the current media.

    The value is the current playback position, expressed in milliseconds since
    the beginning of the media. Periodically changes in the position will be
    indicated with the signal positionChanged(), the interval between updates
    can be set with setNotifyInterval().
*/

/*!
    \property QMediaPlayer::volume
    \brief the current playback volume.

    The playback volume is scaled linearly, ranging from \c 0 (silence) to \c 100 (full volume).
    Values outside this range will be clamped.

    By default the volume is \c 100.

    UI volume controls should usually be scaled nonlinearly. For example, using a logarithmic scale
    will produce linear changes in perceived loudness, which is what a user would normally expect
    from a volume control. See QAudio::convertVolume() for more details.
*/

/*!
    \property QMediaPlayer::muted
    \brief the muted state of the current media.

    The value will be true if the playback volume is muted; otherwise false.
*/

/*!
    \property QMediaPlayer::bufferStatus
    \brief the percentage of the temporary buffer filled before playback begins or resumes, from
    \c 0 (empty) to \c 100 (full).

    When the player object is buffering; this property holds the percentage of
    the temporary buffer that is filled. The buffer will need to reach 100%
    filled before playback can start or resume, at which time mediaStatus() will return
    BufferedMedia or BufferingMedia. If the value is anything lower than \c 100, mediaStatus() will
    return StalledMedia.

    \sa mediaStatus()
*/

/*!
    \property QMediaPlayer::audioAvailable
    \brief the audio availabilty status for the current media.

    As the life time of QMediaPlayer can be longer than the playback of one
    QUrl, this property may change over time, the
    audioAvailableChanged signal can be used to monitor it's status.
*/

/*!
    \property QMediaPlayer::videoAvailable
    \brief the video availability status for the current media.

    If available, the QVideoWidget class can be used to view the video. As the
    life time of QMediaPlayer can be longer than the playback of one
    QUrl, this property may change over time, the
    videoAvailableChanged signal can be used to monitor it's status.

    \sa QVideoWidget, QUrl
*/

/*!
    \property QMediaPlayer::seekable
    \brief the seek-able status of the current media

    If seeking is supported this property will be true; false otherwise. The
    status of this property may change across the life time of the QMediaPlayer
    object, use the seekableChanged signal to monitor changes.
*/

/*!
    \property QMediaPlayer::playbackRate
    \brief the playback rate of the current media.

    This value is a multiplier applied to the media's standard play rate. By
    default this value is 1.0, indicating that the media is playing at the
    standard pace. Values higher than 1.0 will increase the rate of play.
    Values less than zero can be set and indicate the media should rewind at the
    multiplier of the standard pace.

    Not all playback services support change of the playback rate. It is
    framework defined as to the status and quality of audio and video
    while fast forwarding or rewinding.
*/

/*!
    \property QMediaPlayer::audioRole
    \brief the role of the audio stream played by the media player.

    It can be set to specify the type of audio being played, allowing the system to make
    appropriate decisions when it comes to volume, routing or post-processing.

    The audio role must be set before calling setMedia().

    customAudioRole is cleared when this property is set to anything other than
    QAudio::CustomRole.

    \since 5.6
    \sa supportedAudioRoles()
*/

/*!
    \property QMediaPlayer::customAudioRole
    \brief the role of the audio stream played by the media player.

    It can be set to specify the type of audio being played when the backend supports
    audio roles unknown to Qt. Specifying a role allows the system to make appropriate
    decisions when it comes to volume, routing or post-processing.

    The audio role must be set before calling setMedia().

    audioRole is set to QAudio::CustomRole when this property is set.

    \since 5.11
    \sa supportedCustomAudioRoles()
*/

/*!
    \fn void QMediaPlayer::durationChanged(qint64 duration)

    Signal the duration of the content has changed to \a duration, expressed in milliseconds.
*/

/*!
    \fn void QMediaPlayer::positionChanged(qint64 position)

    Signal the position of the content has changed to \a position, expressed in
    milliseconds.
*/

/*!
    \fn void QMediaPlayer::volumeChanged(int volume)

    Signal the playback volume has changed to \a volume.
*/

/*!
    \fn void QMediaPlayer::mutedChanged(bool muted)

    Signal the mute state has changed to \a muted.
*/

/*!
    \fn void QMediaPlayer::videoAvailableChanged(bool videoAvailable)

    Signal the availability of visual content has changed to \a videoAvailable.
*/

/*!
    \fn void QMediaPlayer::audioAvailableChanged(bool available)

    Signals the availability of audio content has changed to \a available.
*/

/*!
    \fn void QMediaPlayer::bufferStatusChanged(int percentFilled)

    Signal the amount of the local buffer filled as a percentage by \a percentFilled.
*/

QT_END_NAMESPACE

#include "moc_qmediaplayer.cpp"
