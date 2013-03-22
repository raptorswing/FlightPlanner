#include "CommonFileHandling.h"

#include "Importers/GPXImporter.h"
#include "Exporters/GPXExporter.h"

#include <QFileDialog>
#include <QFile>
#include <QMessageBox>

//static
bool CommonFileHandling::doExport(const QList<Position> &toExport,
                                  QString destFile,
                                  QWidget * parent)
{
    if (destFile.isEmpty())
    {
        destFile = QFileDialog::getSaveFileName(parent, "Select Export Destination",
                                                QString(), "GPX (*.gpx);;");
    }
    if (destFile.isEmpty())
        return true;

    QFile fp(destFile);
    if (!fp.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(parent, "Error", "Export failed. Failed to open destination file");
        return false;
    }
    QByteArray toWrite;

    QSharedPointer<Exporter> exporter(new GPXExporter(toExport));
    if (!exporter->doExport(&toWrite))
    {
        QMessageBox::warning(parent, "Error", "Export failed. Conversion error.");
        return false;
    }

    if (fp.write(toWrite) != toWrite.size())
    {
        QMessageBox::warning(parent, "Error", "Export failed. Could not write all bytes");
        return false;
    }

    return true;
}

//static
QList<Position> CommonFileHandling::doImport(bool &ok,
                                             QString destFile,
                                             QWidget *parent)
{
    ok = true; //by default
    QList<Position> toRet;

    if (destFile.isEmpty())
    {
        destFile = QFileDialog::getOpenFileName(parent, "Select Import Source",
                                                QString(), "GPX (*.gpx);;");
    }
    if (destFile.isEmpty())
        return toRet;

    QSharedPointer<Importer> importer(new GPXImporter(destFile));
    if (!importer->doImport())
    {
        ok = false;
        QMessageBox::warning(parent, "Error", "Import failed.");
        return toRet;
    }
    toRet = importer->results();

    return toRet;
}
