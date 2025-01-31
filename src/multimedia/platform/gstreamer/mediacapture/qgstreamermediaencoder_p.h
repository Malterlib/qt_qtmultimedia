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


#ifndef QGSTREAMERENCODERCONTROL_H
#define QGSTREAMERENCODERCONTROL_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qplatformmediaencoder_p.h>
#include "qgstreamermediacapture_p.h"
#include "private/qgstreamermetadata_p.h"

#include <QtCore/qurl.h>
#include <QtCore/qdir.h>
#include <qelapsedtimer.h>
#include <qtimer.h>

QT_BEGIN_NAMESPACE

class QMediaMetaData;
class QGstreamerBusHelper;
class QGstreamerMessage;

class QGstreamerMediaEncoder : public QPlatformMediaEncoder
{
    Q_OBJECT

public:
    QGstreamerMediaEncoder(QGstreamerMediaCapture *session, const QGstPipeline &pipeline);
    virtual ~QGstreamerMediaEncoder();

    QUrl outputLocation() const override;
    bool setOutputLocation(const QUrl &sink) override;

    QMediaEncoder::State state() const override;
    QMediaEncoder::Status status() const override;

    qint64 duration() const override;

    void applySettings() override;

    void setEncoderSettings(const QMediaEncoderSettings &settings) override;
    QMediaEncoderSettings encoderSettings() const { return m_settings; }

    void setMetaData(const QMediaMetaData &) override;
    QMediaMetaData metaData() const override;

public slots:
    void setState(QMediaEncoder::State state) override;
    void record();
    void pause();
    void stop();

private slots:
    void updateStatus();
    void handleSessionError(int code, const QString &description);
    void busMessage(const QGstreamerMessage& message);
    void updateDuration();
    void finalize();

private:
    QDir defaultDir() const;
    QString generateFileName(const QDir &dir, const QString &ext) const;

    QUrl m_requestedOutputLocation;
    QUrl m_outputLocation;
    QMediaEncoderSettings m_settings;
    QGstreamerMediaCapture *m_session;
    QGstreamerMetaData m_metaData;
    QMediaEncoder::State m_state = QMediaEncoder::StoppedState;
    QMediaEncoder::Status m_status = QMediaEncoder::StoppedStatus;
    QElapsedTimer m_duration;
    QTimer heartbeat;

    QGstreamerBusHelper *m_busHelper = nullptr;

    QGstPipeline gstPipeline;
    QGstBin gstEncoder;
    QGstElement gstFileSink;

    QGstPad audioSrcPad;
    QGstPad videoSrcPad;
};

QT_END_NAMESPACE

#endif // QGSTREAMERENCODERCONTROL_H
