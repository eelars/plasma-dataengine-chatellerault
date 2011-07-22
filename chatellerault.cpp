#include "chatellerault.h"
#include <Plasma/DataContainer>
#include <KIO/Job>
#include <KUrl>
#include <kdebug.h>


ChatelleraultEngine::ChatelleraultEngine(QObject* parent, const QVariantList& args)
    : Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args)
    setMinimumPollingInterval(333);
}

bool ChatelleraultEngine::sourceRequestEvent(const QString &name)
{
    return updateSourceEvent(name);
}

bool ChatelleraultEngine::updateSourceEvent(const QString &name)
{
    KUrl url;
    QByteArray data;
    // definition of possibles sourcenames
    sourcename["actualites"] = "http://mobinaute.capc-chatellerault.fr/contenus/actualites/";
    sourcename["complementsweb"] = "http://mobinaute.capc-chatellerault.fr/contenus/complementsweb/";
    sourcename["videos"] = "http://mobinaute.capc-chatellerault.fr/video/";
    sourcename["sport"] = "http://mobinaute.capc-chatellerault.fr/agenda/sport/";
    sourcename["culture"] = "http://mobinaute.capc-chatellerault.fr/agenda/culture/";
    sourcename["loisirs"] = "http://mobinaute.capc-chatellerault.fr/agenda/loisirs/";
    // url that will be matched given the sourcename chosen
    url = sourcename.value(name);
    // definition of the job
    m_job = KIO::get(url, KIO::Reload, KIO::HideProgressInfo);

    connect(m_job, SIGNAL(data(KIO::Job*, const QByteArray&)),
            this, SLOT(recv(KIO::Job*, const QByteArray&)));
    connect(m_job, SIGNAL(result(KJob*)), this, SLOT(result(KJob*)));
    return true;
}

void ChatelleraultEngine::recv(KIO::Job*, const QByteArray& data)
{
    m_xml += data;
}

void ChatelleraultEngine::result(KJob *job)
{

    if (job != m_job) {
        qDebug() << "fail! no our job";
        return;
    }

    if (job->error()) {
        qDebug() << "error...";
    } else {
        qDebug() << "job ok";
        QXmlStreamReader reader(m_xml);
        handleStream(reader);
    }

    m_xml.clear();
    m_job = 0;
}

void ChatelleraultEngine::handleStream(QXmlStreamReader &xml)
{
    bool flag = false;
    removeAllSources();
    while (!flag | xml.atEnd() ) {
        if (xml.isStartDocument() )
        {
            flag = true;
            xml.readNext();
            if (xml.name() == "manifestations")
            {
                handleAgendaStream(xml);
            } else if (xml.name() == "videos")
            {
                handleVideoStream(xml);
            } else if (xml.name() == "actualites")
            {
                handleActualitesStream(xml);
            } else if (xml.name() == "complementsweb") {
                handleComplementswebStream(xml);
            } else
            {
                qDebug() << xml.name() << " is not a known name...";
                xml.readNext();
            }
        } else {
            xml.readNext();
        }
    }
}

void ChatelleraultEngine::handleAgendaStream(QXmlStreamReader &xml)
{
    typevalue.clear();
    // definition of stream
    typevalue["titre"] = "string";
    typevalue["debut"] = "date";
    typevalue["fin"] = "date";
    typevalue["horaire"] = "string";
    typevalue["precision-horaire"] = "string";
    typevalue["lieu"] = "string";
    typevalue["theme"] = "string";
    typevalue["sous-theme"] = "string";
    typevalue["site-internet"] = "url";
    typevalue["public"] = "string";
    typevalue["description"] = "string";
    typevalue["image"] = "url";
    typevalue["tarif"] = "string";
    typevalue["pass-culture"] = "bool";
    // parsing
    feedData(xml, "manifestation", typevalue);
}


void ChatelleraultEngine::handleVideoStream(QXmlStreamReader& xml)
{
    typevalue.clear();
    // definition of stream
    typevalue["titre"] = "string";
    typevalue["date"] = "date";
    typevalue["image"] = "url";
    typevalue["fichier"] = "url";
    typevalue["description"] = "string";
    // parsing
    feedData(xml, "video", typevalue);

}

void ChatelleraultEngine::handleComplementswebStream(QXmlStreamReader &xml)
{
    typevalue.clear();
    // definition of stream
    typevalue["numero"] = "string";
    typevalue["categorie"] = "string";
    typevalue["titre"] = "string";
    typevalue["chapo"] = "string";
    typevalue["texte"] = "string";
    typevalue["image"] = "url";
    typevalue["info-pratiques"] = "string";
    // parsing
    feedData(xml, "complementweb", typevalue);
}


void ChatelleraultEngine::handleActualitesStream(QXmlStreamReader &xml)
{
    typevalue.clear();
    // definition of stream
    typevalue["type"] = "string";
    typevalue["date"] = "date";
    typevalue["titre"] = "string";
    typevalue["resume"] = "string";
    typevalue["description"] = "string";
    typevalue["image"] = "url";
    // parsing
    feedData(xml, "actualite", typevalue);
}



void ChatelleraultEngine::feedData(QXmlStreamReader &xml, QString source, const QMap <QString, QString> &streamvalues)
{
    int id = 0;
    xml.readNext();
    while (!xml.atEnd() && !xml.hasError())
    {
        if (xml.isStartElement())
        {
            if(xml.name() == source)
            {
                id += 1;
                QString strId = QString::number(id);
                while (xml.readNextStartElement() && xml.name() != source)
                {
                    QString key = xml.name().toString();
                    if(streamvalues.contains(key))
                    {
                        QString strValue = xml.readElementText();
                        QString value = streamvalues.value(key);
                        if (value == "string")
                        {
                            setData(strId, key, strValue);
                        }
                        if (value == "date") {
                            QDate dateValue = QDate::fromString(strValue, "dd/MM/yyyy");
                            setData(strId, key, dateValue);
                        }
                        if (value == "url") {
                            KUrl urlValue = strValue;
                            setData(strId, key, urlValue.prettyUrl());
                        }
                        if (value == "bool") {
                            bool boolValue = false;
                            if (strValue == "true") {
                                boolValue = true;
                            }
                            setData(strId, key, boolValue);
                        }
                    }

                }
                qDebug() << "object's end";
            }
        }
        xml.readNext();
    }

    if(xml.hasError()) {
        qDebug() << "xml error";
        qDebug() << xml.lineNumber();
        qDebug() << xml.errorString();
    }
    qDebug() << "document parsing is finished";
    xml.clear();
}

K_EXPORT_PLASMA_DATAENGINE(chatellerault, ChatelleraultEngine)
#include "chatellerault.moc"
