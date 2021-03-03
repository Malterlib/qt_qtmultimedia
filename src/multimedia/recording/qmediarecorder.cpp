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

#include "qmediarecorder.h"
#include "qmediarecorder_p.h"

#include <private/qplatformmediarecorder_p.h>
#include <qaudiodeviceinfo.h>
#include <qcamera.h>
#include <private/qplatformcamera_p.h>
#include <private/qplatformmediaintegration_p.h>
#include <private/qplatformmediacapture_p.h>

#include <QtCore/qdebug.h>
#include <QtCore/qurl.h>
#include <QtCore/qstringlist.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qtimer.h>

#include <qaudioformat.h>

QT_BEGIN_NAMESPACE

/*!
    \class QMediaRecorder
    \inmodule QtMultimedia
    \ingroup multimedia
    \ingroup multimedia_recording

    \brief The QMediaRecorder class is used for the recording of media content.

    The QMediaRecorder class is a high level media recording class.  It's not
    intended to be used alone but for accessing the media recording functions
    of other media objects, like QCamera.

    \snippet multimedia-snippets/media.cpp Media recorder
*/


#define ENUM_NAME(c,e,v) (c::staticMetaObject.enumerator(c::staticMetaObject.indexOfEnumerator(e)).valueToKey((v)))

void QMediaRecorderPrivate::_q_stateChanged(QMediaRecorder::State ps)
{
    Q_Q(QMediaRecorder);

    if (ps == QMediaRecorder::RecordingState)
        notifyTimer->start();
    else
        notifyTimer->stop();

//    qDebug() << "Recorder state changed:" << ENUM_NAME(QMediaRecorder,"State",ps);
    if (state != ps) {
        emit q->stateChanged(ps);
    }

    state = ps;
}


void QMediaRecorderPrivate::_q_error(int error, const QString &errorString)
{
    Q_Q(QMediaRecorder);

    this->error = QMediaRecorder::Error(error);
    this->errorString = errorString;

    emit q->error(this->error);
}

void QMediaRecorderPrivate::_q_updateActualLocation(const QUrl &location)
{
    if (actualLocation != location) {
        actualLocation = location;
        emit q_func()->actualLocationChanged(actualLocation);
    }
}

void QMediaRecorderPrivate::_q_notify()
{
    emit q_func()->durationChanged(q_func()->duration());
}

void QMediaRecorderPrivate::_q_updateNotifyInterval(int ms)
{
    notifyTimer->setInterval(ms);
}

void QMediaRecorderPrivate::applySettingsLater()
{
    if (control && !settingsChanged) {
        settingsChanged = true;
        QMetaObject::invokeMethod(q_func(), "_q_applySettings", Qt::QueuedConnection);
    }
}

void QMediaRecorderPrivate::_q_applySettings()
{
    if (control && settingsChanged) {
        settingsChanged = false;
        control->applySettings();
    }
}

/*!
    Constructs a media recorder which records the media produced by a microphone and camera.
*/

QMediaRecorder::QMediaRecorder(QMediaRecorder::CaptureMode mode, QObject *parent)
    : QObject(parent),
      d_ptr(new QMediaRecorderPrivate)
{
    Q_D(QMediaRecorder);
    d->q_ptr = this;

    d->notifyTimer = new QTimer(this);
    connect(d->notifyTimer, SIGNAL(timeout()), SLOT(_q_notify()));

    if (mode != AudioOnly) {
        setCamera(new QCamera(this));
    } else {
        auto *captureIface = QPlatformMediaIntegration::instance()->createCaptureSession(mode);
        d->control = captureIface->mediaRecorderControl();
    }
}

QMediaRecorder::QMediaRecorder(QCamera *camera, QObject *parent)
    : QObject(parent),
      d_ptr(new QMediaRecorderPrivate)
{
    Q_D(QMediaRecorder);
    d->q_ptr = this;

    d->notifyTimer = new QTimer(this);
    connect(d->notifyTimer, SIGNAL(timeout()), SLOT(_q_notify()));

    setCamera(camera);
}

/*!
    Destroys a media recorder object.
*/

QMediaRecorder::~QMediaRecorder()
{
    delete d_ptr;
}

int QMediaRecorder::notifyInterval() const
{
    return d_func()->notifyTimer->interval();
}

void QMediaRecorder::setNotifyInterval(int milliSeconds)
{
    Q_D(QMediaRecorder);

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

void QMediaRecorder::addPropertyWatch(QByteArray const &name)
{
    Q_D(QMediaRecorder);

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

void QMediaRecorder::removePropertyWatch(QByteArray const &name)
{
    Q_D(QMediaRecorder);

    int index = metaObject()->indexOfProperty(name.constData());

    if (index != -1) {
        d->notifyProperties.remove(index);

        if (d->notifyProperties.isEmpty())
            d->notifyTimer->stop();
    }
}

/*!
    \internal
*/
bool QMediaRecorder::setCamera(QCamera *object)
{
    Q_D(QMediaRecorder);
    Q_ASSERT(!d->camera);

    d->camera = object;

    auto *service = d->camera->captureInterface();
    Q_ASSERT(service);

    d->notifyTimer->setInterval(notifyInterval());
    connect(this, SIGNAL(notifyIntervalChanged(int)), SLOT(_q_updateNotifyInterval(int)));

    d->control = service->mediaRecorderControl();
    Q_ASSERT(d->control);

    connect(d->control, SIGNAL(stateChanged(QMediaRecorder::State)),
            this, SLOT(_q_stateChanged(QMediaRecorder::State)));

    connect(d->control, SIGNAL(statusChanged(QMediaRecorder::Status)),
            this, SIGNAL(statusChanged(QMediaRecorder::Status)));

    connect(d->control, SIGNAL(mutedChanged(bool)),
            this, SIGNAL(mutedChanged(bool)));

    connect(d->control, SIGNAL(volumeChanged(qreal)),
            this, SIGNAL(volumeChanged(qreal)));

    connect(d->control, SIGNAL(durationChanged(qint64)),
            this, SIGNAL(durationChanged(qint64)));

    connect(d->control, SIGNAL(actualLocationChanged(QUrl)),
            this, SLOT(_q_updateActualLocation(QUrl)));

    connect(d->control, SIGNAL(error(int,QString)),
            this, SLOT(_q_error(int,QString)));

    connect(d->control, SIGNAL(metaDataChanged()),
            this, SIGNAL(metaDataChanged()));

    d->applySettingsLater();

    return true;
}

/*!
    \property QMediaRecorder::outputLocation
    \brief the destination location of media content.

    Setting the location can fail, for example when the service supports only
    local file system locations but a network URL was passed. If the service
    does not support media recording this setting the output location will
    always fail.

    The \a location can be relative or empty;
    in this case the recorder uses the system specific place and file naming scheme.
    After recording has stated, QMediaRecorder::outputLocation() returns the actual output location.
*/

/*!
    \property QMediaRecorder::actualLocation
    \brief the actual location of the last media content.

    The actual location is usually available after recording starts,
    and reset when new location is set or new recording starts.
*/

/*!
    Returns true if media recorder service ready to use.

    \sa availabilityChanged()
*/
bool QMediaRecorder::isAvailable() const
{
    return d_func()->control != nullptr;
}

QUrl QMediaRecorder::outputLocation() const
{
    return d_func()->control ? d_func()->control->outputLocation() : QUrl();
}

bool QMediaRecorder::setOutputLocation(const QUrl &location)
{
    Q_D(QMediaRecorder);
    d->actualLocation.clear();
    return d->control ? d->control->setOutputLocation(location) : false;
}

QUrl QMediaRecorder::actualLocation() const
{
    return d_func()->actualLocation;
}

/*!
    Returns the current media recorder state.

    \sa QMediaRecorder::State
*/

QMediaRecorder::State QMediaRecorder::state() const
{
    return d_func()->control ? QMediaRecorder::State(d_func()->control->state()) : StoppedState;
}

/*!
    Returns the current media recorder status.

    \sa QMediaRecorder::Status
*/

QMediaRecorder::Status QMediaRecorder::status() const
{
    return d_func()->control ? QMediaRecorder::Status(d_func()->control->status()) : UnavailableStatus;
}

/*!
    Returns the current error state.

    \sa errorString()
*/

QMediaRecorder::Error QMediaRecorder::error() const
{
    return d_func()->error;
}

/*!
    Returns a string describing the current error state.

    \sa error()
*/

QString QMediaRecorder::errorString() const
{
    return d_func()->errorString;
}

/*!
    \property QMediaRecorder::duration

    \brief the recorded media duration in milliseconds.
*/

qint64 QMediaRecorder::duration() const
{
    return d_func()->control ? d_func()->control->duration() : 0;
}

/*!
    \property QMediaRecorder::muted

    \brief whether a recording audio stream is muted.
*/

bool QMediaRecorder::isMuted() const
{
    return d_func()->control ? d_func()->control->isMuted() : false;
}

void QMediaRecorder::setMuted(bool muted)
{
    Q_D(QMediaRecorder);

    if (d->control)
        d->control->setMuted(muted);
}

/*!
    \property QMediaRecorder::volume

    \brief the current recording audio volume.

    The volume is scaled linearly from \c 0.0 (silence) to \c 1.0 (full volume). Values outside this
    range will be clamped.

    The default volume is \c 1.0.

    UI volume controls should usually be scaled nonlinearly. For example, using a logarithmic scale
    will produce linear changes in perceived loudness, which is what a user would normally expect
    from a volume control. See QAudio::convertVolume() for more details.
*/

qreal QMediaRecorder::volume() const
{
    return d_func()->control ? d_func()->control->volume() : 1.0;
}

/*!
    Sets the encoder settings to \a settings.

    \sa QMediaEncoderSettings
*/
void QMediaRecorder::setEncoderSettings(const QMediaEncoderSettings &settings)
{
    Q_D(QMediaRecorder);

    d->encoderSettings = settings;
    d->control->setEncoderSettings(settings);
    d->applySettingsLater();
}

/*!
    Returns the current encoder settings.

    \sa QMediaEncoderSettings
*/
QMediaEncoderSettings QMediaRecorder::encoderSettings() const
{
    return d_func()->encoderSettings;
}


void QMediaRecorder::setVolume(qreal volume)
{
    Q_D(QMediaRecorder);

    if (d->control) {
        volume = qMax(qreal(0.0), volume);
        d->control->setVolume(volume);
    }
}

/*!
    Start recording.

    While the recorder state is changed immediately to QMediaRecorder::RecordingState,
    recording may start asynchronously, with statusChanged(QMediaRecorder::RecordingStatus)
    signal emitted when recording starts.

    If recording fails error() signal is emitted
    with recorder state being reset back to QMediaRecorder::StoppedState.
*/

void QMediaRecorder::record()
{
    Q_D(QMediaRecorder);

    d->actualLocation.clear();

    if (d->settingsChanged)
        d->_q_applySettings();

    // reset error
    d->error = NoError;
    d->errorString = QString();

    if (d->control)
        d->control->setState(RecordingState);
}

/*!
    Pause recording.

    The recorder state is changed to QMediaRecorder::PausedState.

    Depending on platform recording pause may be not supported,
    in this case the recorder state stays unchanged.
*/

void QMediaRecorder::pause()
{
    Q_D(QMediaRecorder);
    if (d->control)
        d->control->setState(PausedState);
}

/*!
    Stop recording.

    The recorder state is changed to QMediaRecorder::StoppedState.
*/

void QMediaRecorder::stop()
{
    Q_D(QMediaRecorder);
    if (d->control)
        d->control->setState(StoppedState);
}

/*!
    \enum QMediaRecorder::State

    \value StoppedState    The recorder is not active.
        If this is the state after recording then the actual created recording has
        finished being written to the final location and is ready on all platforms
        except on Android. On Android, due to platform limitations, there is no way
        to be certain that the recording has finished writing to the final location.
    \value RecordingState  The recording is requested.
    \value PausedState     The recorder is paused.
*/

/*!
    \enum QMediaRecorder::Status

    \value UnavailableStatus
        The recorder is not available or not supported by connected media object.
    \value UnloadedStatus
        The recorder is avilable but not loaded.
    \value LoadingStatus
        The recorder is initializing.
    \value LoadedStatus
        The recorder is initialized and ready to record media.
    \value StartingStatus
        Recording is requested but not active yet.
    \value RecordingStatus
        Recording is active.
    \value PausedStatus
        Recording is paused.
    \value FinalizingStatus
        Recording is stopped with media being finalized.
*/

/*!
    \enum QMediaRecorder::Error

    \value NoError         No Errors.
    \value ResourceError   Device is not ready or not available.
    \value FormatError     Current format is not supported.
    \value OutOfSpaceError No space left on device.
*/

/*!
    \property QMediaRecorder::state
    \brief The current state of the media recorder.

    The state property represents the user request and is changed synchronously
    during record(), pause() or stop() calls.
    Recorder state may also change asynchronously when recording fails.
*/

/*!
    \property QMediaRecorder::status
    \brief The current status of the media recorder.

    The status is changed asynchronously and represents the actual status
    of media recorder.
*/

/*!
    \fn QMediaRecorder::stateChanged(State state)

    Signals that a media recorder's \a state has changed.
*/

/*!
    \fn QMediaRecorder::durationChanged(qint64 duration)

    Signals that the \a duration of the recorded media has changed.
*/

/*!
    \fn QMediaRecorder::actualLocationChanged(const QUrl &location)

    Signals that the actual \a location of the recorded media has changed.
    This signal is usually emitted when recording starts.
*/

/*!
    \fn QMediaRecorder::error(QMediaRecorder::Error error)

    Signals that an \a error has occurred.
*/

/*!
    \fn QMediaRecorder::availabilityChanged(bool available)

    Signals that the media recorder is now available (if \a available is true), or not.
*/

/*!
    \fn QMediaRecorder::mutedChanged(bool muted)

    Signals that the \a muted state has changed. If true the recording is being muted.
*/

/*!
    Returns the metaData associated with the recording.
*/
QMediaMetaData QMediaRecorder::metaData() const
{
    Q_D(const QMediaRecorder);

    return d->control ? d->control->metaData() : QMediaMetaData{};
}

/*!
    Sets the meta data tp \a metaData.

    \note To ensure that meta data is set corretly, it should be set before starting the recording.
    Once the recording is stopped, any meta data set will be attached to the next recording.
*/
void QMediaRecorder::setMetaData(const QMediaMetaData &metaData)
{
    Q_D(QMediaRecorder);

    if (d->control)
        d->control->setMetaData(metaData);
}

void QMediaRecorder::addMetaData(const QMediaMetaData &metaData)
{
    auto data = this->metaData();
    // merge data
    for (const auto &k : metaData.keys())
        data.insert(k, metaData.value(k));
    setMetaData(data);
}

/*!
    \fn QMediaRecorder::metaDataChanged()

    Signals that a media object's meta-data has changed.

    If multiple meta-data elements are changed,
    metaDataChanged(const QString &key, const QVariant &value) signal is emitted
    for each of them with metaDataChanged() changed emitted once.
*/

/*!
    \property QMediaRecorder::audioInput
    \brief the active audio input name.

*/

/*!
    Returns the active audio input.
*/

QAudioDeviceInfo QMediaRecorder::audioInput() const
{
    Q_D(const QMediaRecorder);

    return d->control->audioInput();
}

/*!
    Returns information about the active video input.
*/
QCameraInfo QMediaRecorder::videoInput() const
{
    Q_D(const QMediaRecorder);

    return d->camera ? d->camera->cameraInfo() : QCameraInfo();
}

QCamera *QMediaRecorder::camera() const
{
    Q_D(const QMediaRecorder);
    return d->camera;
}

/*!
    Set the active audio input to \a device.
*/

bool QMediaRecorder::setAudioInput(const QAudioDeviceInfo &device)
{
    Q_D(QMediaRecorder);

    if (d->control && d->control->setAudioInput(device)) {
        audioInputChanged();
        return true;
    }
    return false;
}

/*!
    \fn QMediaRecorder::audioInputChanged(const QString& name)

    Signal emitted when active audio input changes to \a name.
*/

QT_END_NAMESPACE

#include "moc_qmediarecorder.cpp"
