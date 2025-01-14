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

#ifndef PLAYLISTFILEPARSER_P_H
#define PLAYLISTFILEPARSER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qtmultimediaglobal.h"
#include <QtCore/qobject.h>
#include <qmediaplaylist.h>

QT_BEGIN_NAMESPACE

class QIODevice;
class QUrl;
class QNetworkRequest;

class QPlaylistFileParserPrivate;

class Q_MULTIMEDIA_EXPORT QPlaylistFileParser : public QObject
{
    Q_OBJECT
public:
    QPlaylistFileParser(QObject *parent = nullptr);
    ~QPlaylistFileParser();

    enum FileType
    {
        UNKNOWN,
        M3U,
        M3U8, // UTF-8 version of M3U
        PLS
    };

    void start(const QUrl &media, QIODevice *stream = nullptr, const QString &mimeType = QString());
    void start(const QUrl &request, const QString &mimeType = QString());
    void start(QIODevice *stream, const QString &mimeType = QString());
    void abort();

    QList<QUrl> playlist;

Q_SIGNALS:
    void newItem(const QVariant& content);
    void finished();
    void error(QMediaPlaylist::Error err, const QString& errorMsg);

private Q_SLOTS:
    void handleData();
    void handleError();

private:

    static FileType findByMimeType(const QString &mime);
    static FileType findBySuffixType(const QString &suffix);
    static FileType findByDataHeader(const char *data, quint32 size);
    static FileType findPlaylistType(QIODevice *device,
                                     const QString& mime);
    static FileType findPlaylistType(const QString &suffix,
                                     const QString& mime,
                                     const char *data = nullptr,
                                     quint32 size = 0);

    Q_DISABLE_COPY(QPlaylistFileParser)
    Q_DECLARE_PRIVATE(QPlaylistFileParser)
    QScopedPointer<QPlaylistFileParserPrivate> d_ptr;
};

QT_END_NAMESPACE

#endif // PLAYLISTFILEPARSER_P_H
