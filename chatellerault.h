// the following header is required by the LGPL because
// we are using the actual time engine code
/*
 *   Copyright 2007 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

// a standard include guard to prevent problems if the
// header is included multiple times
#ifndef CHATELLERAULTENGINE_H
#define CHATELLERAULTENGINE_H

// We need the DataEngine header, since we are inheriting it
#include <Plasma/DataEngine>

#include <kurl.h>
#include <kio/jobclasses.h>
#include <QXmlStreamReader>
#include <QMap>

/**
 * This engine provides the current date and time for a given
 * timezone.
 *
 * "Local" is a special source that is an alias for the current
 * timezone.
 */
class ChatelleraultEngine : public Plasma::DataEngine
{
    // required since Plasma::DataEngine inherits QObject
    Q_OBJECT

    public:
        // every engine needs a constructor with these arguments
        ChatelleraultEngine(QObject* parent, const QVariantList& args);

    protected:
        // this virtual function is called when a new source is requested
        bool sourceRequestEvent(const QString& name);

        // this virtual function is called when an automatic update
        // is triggered for an existing source (ie: when a valid update
        // interval is set when requesting a source)
        bool updateSourceEvent(const QString& source);

    private slots:

        void recv(KIO::Job*, const QByteArray& data);
        void result(KJob*);

    private:
        void handleStream(QXmlStreamReader &xml);
        void handleAgendaStream(QXmlStreamReader &xml);
        void handleVideoStream(QXmlStreamReader &xml);
        void handleActualitesStream(QXmlStreamReader &xml);
        void handleComplementswebStream(QXmlStreamReader &xml);
        void feedData(QXmlStreamReader &xml, QString source, const QMap <QString, QString> &streamvalues);

        QMap<QString, QString> sourcename;
        QMap<QString, QString> typevalue;
        QByteArray m_xml;
        Plasma::DataEngine::Data m_tempData;
        KIO::Job * m_job;
        QString m_id;

};

#endif