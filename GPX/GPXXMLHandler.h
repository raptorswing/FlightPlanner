#ifndef GPXXMLHANDLER_H
#define GPXXMLHANDLER_H

#include <QXmlDefaultHandler>
#include <QList>
#include <QStack>

#include "GPXPoint.h"

class GPXXMLHandler : public QXmlDefaultHandler
{
public:
    GPXXMLHandler();
    ~GPXXMLHandler();

    //virtual from QXmlDefaultHandler
    bool startElement(const QString &namespaceURI,
                      const QString &localName,
                      const QString &qName,
                      const QXmlAttributes &atts);

    //virtual from QXmlDefaultHandler
    bool endElement(const QString &namespaceURI,
                    const QString &localName,
                    const QString &qName);

    //virtual from QXmlDefaultHandler
    bool characters(const QString &ch);

    //pure-virtul from QXmlContentHandler
    QString errorString() const;

    /**
     * @brief Returns the finished list of points after parsing is done
     *
     */
    QList<GPXPoint> getPoints();

private:
    /**
     * @brief Handle the attributes of a trkpt. Sets the attributes on the currentPoint
     *
     * @param atts
     */
    void processAttributes(const QXmlAttributes &atts);

    QList<GPXPoint> points;
    GPXPoint * currentPoint;
    QStack<QString> lastQName;

    QString errorMessage;
};

#endif // GPXXMLHANDLER_H
