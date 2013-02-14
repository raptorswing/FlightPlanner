#include "GPX.h"
#include "GPXXMLHandler.h"

#include <QFile>
#include <QSharedPointer>
#include <QTextStream>
#include <QDomDocument>
#include <QtDebug>


GPX::GPX()
{
}

GPX::~GPX()
{

}

void GPX::setPoints(QList<GPXPoint> nPoints)
{
    _points = nPoints;
}

const QList<GPXPoint> &GPX::points() const
{
    return _points;
}

void GPX::appendPoint(GPXPoint point)
{
    _points.append(point);
}

bool GPX::toXML(QByteArray *dest, QString *)
{
    QTextStream stream(dest);
    QDomDocument doc;

    QDomElement root = doc.createElement("gpx");
    root.setAttribute("version",1.1);
    root.setAttribute("creator","CommStation");
    root.setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance");
    root.setAttribute("xmlns","http://www.topografix.com/GPX/1/1");
    root.setAttribute("xsi:schemaLocation","http://www.topografix.com/GPX/1/1 http://www.topografix.com/GPX/1/1/gpx.xsd");
    doc.appendChild(root);

    QDomElement track = doc.createElement("trk");
    root.appendChild(track);
    QDomElement segment = doc.createElement("trkseg");
    track.appendChild(segment);


    foreach(GPXPoint point, _points)
    {
        QDomElement trackPoint = doc.createElement("trkpt");
        trackPoint.setAttribute("lat",point.latitude);
        trackPoint.setAttribute("lon",point.longitude);

        QDomElement timeNode = doc.createElement("time");
        QDomText timeNodeText = doc.createTextNode(point.time.toString(Qt::ISODate));
        timeNode.appendChild(timeNodeText);
        trackPoint.appendChild(timeNode);

        QDomElement heightNode = doc.createElement("ele");
        QDomText heightNodeText = doc.createTextNode(QString::number(point.height));
        heightNode.appendChild(heightNodeText);
        trackPoint.appendChild(heightNode);

        segment.appendChild(trackPoint);

    }

    doc.save(stream,0);
    return true;
}

//static
bool GPX::parseGPXFile(QString filePath, GPX *output, QString *messages)
{
    QSharedPointer<QFile> fp(new QFile(filePath));
    if (!fp->exists())
    {
        if (messages)
            *messages = "No such file:" + filePath;
        return false;
    }

    QSharedPointer<GPXXMLHandler> handler(new GPXXMLHandler());
    QXmlSimpleReader reader;
    QXmlInputSource source(fp.data());
    reader.setContentHandler(handler.data());
    if (!reader.parse(source))
    {
        *messages = handler->errorString();
        return false;
    }

    fp->close();
    (*output).setPoints(handler->getPoints());
    return true;
}
