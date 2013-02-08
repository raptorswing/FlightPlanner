#ifndef GPX_H
#define GPX_H

#include "GPX_global.h"
#include "GPXPoint.h"

/**
 * @brief Represents a simple GPX file. Parses all GPX files as if they
 * only had a single track and segment.
 *
 */
class GPXSHARED_EXPORT GPX
{
public:
    GPX();
    ~GPX();

    /**
     * @brief Sets the points that comprise a GPX log
     *
     * @param QList<GPXPoint>
     */
    void setPoints(QList<GPXPoint>);

    /**
     * @brief Append a single point to the GPX track
     *
     * @param GPXPoint
     */
    void appendPoint(GPXPoint);

    /**
     * @brief Serializes the GPX object to XML. Outputs to the given
     * QByteArray pointer. Returns true on success. On failure, an error
     * will be written to the errorMessage pointer, if provided.
     *
     * @param dest
     * @param errorMessage
     */
    bool toXML(QByteArray * dest, QString * errorMessage = 0);


    /**
     * @brief Given a filename/path, parses a .gpx file from the filesystem.
     * Returns true on success, false on failure. The parsed GPX object will be
     * assigned to the pointer you provide in the output argument.
     * If the optional messages argument is supplied and non-null, error messages
     * will be placed there.
     *
     * @param filePath path to the the .gpx file you want parsed
     * @param output where the parsed GPX object will be placed, if successful
     * @param messages where erros messages will be written if something bad happens
     */
    static bool parseGPXFile(QString filePath, GPX * output, QString * messages =0);

private:
    QList<GPXPoint> points;
};

#endif // GPX_H
