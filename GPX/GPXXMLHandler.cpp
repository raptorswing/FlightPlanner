#include "GPXXMLHandler.h"

#include <QDateTime>

GPXXMLHandler::GPXXMLHandler() :
    currentPoint(0)
{
}

GPXXMLHandler::~GPXXMLHandler()
{

}

//virtual from QXmlDefaultHandler
bool GPXXMLHandler::startElement(const QString &,
                                 const QString &,
                                 const QString &qName,
                                 const QXmlAttributes &atts)
{
    if (qName.toLower() == "trkpt")
    {
        if (this->currentPoint != 0)
        {
            this->errorMessage = "Mismatched element open/close tags - there is already an open tag";
            return false;
        }
        this->currentPoint = new GPXPoint();
        this->processAttributes(atts);
    }
    this->lastQName.push(qName);
    return true;
}

//virtual from QXmlDefaultHandler
bool GPXXMLHandler::endElement(const QString &,
                               const QString &,
                               const QString &qName)
{
    if (qName == "trkpt")
    {
        if (this->currentPoint == 0)
        {
            this->errorMessage = "Mismatched element open/close tags - there is no open tag";
            return false;
        }
        this->points.append(*this->currentPoint);
        delete this->currentPoint;
        this->currentPoint = 0;
    }
    this->lastQName.pop();
    return true;
}

//virtual from QXmlDefaultHandler
bool GPXXMLHandler::characters(const QString &ch)
{
    if (!this->lastQName.isEmpty() && this->lastQName.top() == "ele" && this->currentPoint != 0)
    {
        bool ok = true;
        double ele = ch.toDouble(&ok);
        if (!ok)
        {
            this->errorMessage = "Failed to parse " + ch + " as double";
            return false;
        }
        this->currentPoint->height = ele;
    }
    else if (!this->lastQName.isEmpty() && this->lastQName.top() == "time" && this->currentPoint != 0)
    {
        QDateTime time = QDateTime::fromString(ch,Qt::ISODate);
        if (!time.isValid())
        {
            this->errorMessage = "Failed to parse " + ch + " as a time";
            return false;
        }
        this->currentPoint->time = time;
    }
    return true;
}

//pure-virtul from QXmlContentHandler
QString GPXXMLHandler::errorString() const
{
    return this->errorMessage;
}

QList<GPXPoint> GPXXMLHandler::getPoints()
{
    return this->points;
}

//private
void GPXXMLHandler::processAttributes(const QXmlAttributes &atts)
{
    for (int i = 0; i < atts.count(); i++)
    {
        QString qName = atts.qName(i);
        if (qName.toLower() == "lat")
        {
            bool check = true;
            double lat = atts.value(i).toDouble(&check);
            if (!check)
            {
                this->errorMessage = "Failed to parse:" + atts.value(i) + " as a double";
                return;
            }

            this->currentPoint->latitude = lat;
        }
        else if (qName.toLower() == "lon")
        {
            bool check = true;
            double lon = atts.value(i).toDouble(&check);
            if (!check)
            {
                this->errorMessage = "Failed to parse:" + atts.value(i) + " as a double";
                return;
            }

            this->currentPoint->longitude = lon;
        }
    }
}
