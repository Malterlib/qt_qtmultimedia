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

#ifndef QCAMERAEXPOSURE_H
#define QCAMERAEXPOSURE_H

#include <QtCore/qobject.h>
#include <QtMultimedia/qtmultimediaglobal.h>
#include <QtMultimedia/qmediaenumdebug.h>

QT_BEGIN_NAMESPACE


class QCamera;
class QPlatformCamera;
class QCameraExposurePrivate;

class Q_MULTIMEDIA_EXPORT QCameraExposure : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal aperture READ aperture NOTIFY apertureChanged)
    Q_PROPERTY(qreal shutterSpeed READ shutterSpeed NOTIFY shutterSpeedChanged)
    Q_PROPERTY(int isoSensitivity READ isoSensitivity NOTIFY isoSensitivityChanged)
    Q_PROPERTY(qreal exposureCompensation READ exposureCompensation WRITE setExposureCompensation NOTIFY exposureCompensationChanged)
    Q_PROPERTY(bool flashReady READ isFlashReady NOTIFY flashReady)
    Q_PROPERTY(QCameraExposure::FlashMode flashMode READ flashMode WRITE setFlashMode)
    Q_PROPERTY(QCameraExposure::TorchMode torchMode READ torchMode WRITE setTorchMode)
    Q_PROPERTY(QCameraExposure::ExposureMode exposureMode READ exposureMode WRITE setExposureMode)

    Q_ENUMS(FlashMode)
    Q_ENUMS(ExposureMode)
public:
    enum FlashMode {
        FlashOff,
        FlashOn,
        FlashAuto
    };

    enum TorchMode {
        TorchOff,
        TorchOn,
        TorchAuto
    };

    enum ExposureMode {
        ExposureAuto,
        ExposureManual,
        ExposurePortrait,
        ExposureNight,
        ExposureSports,
        ExposureSnow,
        ExposureBeach,
        ExposureAction,
        ExposureLandscape,
        ExposureNightPortrait,
        ExposureTheatre,
        ExposureSunset,
        ExposureSteadyPhoto,
        ExposureFireworks,
        ExposureParty,
        ExposureCandlelight,
        ExposureBarcode
    };

    bool isAvailable() const;

    FlashMode flashMode() const;
    bool isFlashModeSupported(FlashMode mode) const;
    bool isFlashReady() const;

    TorchMode torchMode() const;
    bool isTorchModeSupported(TorchMode mode) const;

    ExposureMode exposureMode() const;
    bool isExposureModeSupported(ExposureMode mode) const;

    qreal exposureCompensation() const;

    int isoSensitivity() const;
    qreal aperture() const;
    qreal shutterSpeed() const;

    int requestedIsoSensitivity() const;
    qreal requestedAperture() const;
    qreal requestedShutterSpeed() const;

    QList<int> supportedIsoSensitivities(bool *continuous = nullptr) const;
    QList<qreal> supportedApertures(bool *continuous = nullptr) const;
    QList<qreal> supportedShutterSpeeds(bool *continuous = nullptr) const;

public Q_SLOTS:
    void setFlashMode(FlashMode mode);
    void setTorchMode(TorchMode mode);
    void setExposureMode(ExposureMode mode);

    void setExposureCompensation(qreal ev);

    void setManualIsoSensitivity(int iso);
    void setAutoIsoSensitivity();

    void setManualAperture(qreal aperture);
    void setAutoAperture();

    void setManualShutterSpeed(qreal seconds);
    void setAutoShutterSpeed();

Q_SIGNALS:
    void flashReady(bool);

    void apertureChanged(qreal);
    void apertureRangeChanged();
    void shutterSpeedChanged(qreal speed);
    void shutterSpeedRangeChanged();
    void isoSensitivityChanged(int);
    void exposureCompensationChanged(qreal);

protected:
    virtual ~QCameraExposure();

private:
    friend class QCamera;
    friend class QCameraPrivate;
    explicit QCameraExposure(QCamera *parent, QPlatformCamera *cameraControl);

    Q_DISABLE_COPY(QCameraExposure)
    Q_DECLARE_PRIVATE(QCameraExposure)
    Q_PRIVATE_SLOT(d_func(), void _q_exposureParameterChanged(int))
    Q_PRIVATE_SLOT(d_func(), void _q_exposureParameterRangeChanged(int))
    QCameraExposurePrivate *d_ptr;
};

QT_END_NAMESPACE

Q_MEDIA_ENUM_DEBUG(QCameraExposure, ExposureMode)
Q_MEDIA_ENUM_DEBUG(QCameraExposure, FlashMode)

#endif // QCAMERAEXPOSURE_H
