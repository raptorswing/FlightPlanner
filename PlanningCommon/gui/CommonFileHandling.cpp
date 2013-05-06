#include "CommonFileHandling.h"

#include "CommonWindowHandling.h"

#include "Importers/GPXImporter.h"
#include "Exporters/GPXExporter.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>

//private
QString CommonFileHandling::_resultPrefix = QString("");

//static
bool CommonFileHandling::doExport(const Wayset &toExport,
                                  QString destFile,
                                  QWidget * parent)
{
    if (destFile.isEmpty())
    {
        destFile = CommonWindowHandling::getExportSolutionFilename(parent);
    }
    if (destFile.isEmpty())
        return false;

    QFile fp(destFile);
    if (!fp.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(parent, "Error", "Export failed. Failed to open destination file");
        return false;
    }
    QByteArray toWrite;

    QSharedPointer<Exporter> exporter(Exporter::getExporter(destFile));
    if (exporter.isNull())
        QMessageBox::warning(parent, "Error", "Export failed. Received bad exporter.");
    else if (!exporter->doExport(toExport))
        QMessageBox::warning(parent, "Error", "Export failed. Conversion error.");
    else if (fp.write(toWrite) != toWrite.size())
        QMessageBox::warning(parent, "Error", "Export failed. Could not write all bytes");
    else
        return true;

    return false;
}

//static
Wayset CommonFileHandling::doImport(bool &success,
                                    QString srcFile,
                                    QWidget *parent)
{
    success = false; //by default
    Wayset toRet;

    if (srcFile.isEmpty())
    {
        srcFile = CommonWindowHandling::getImportSolutionFilename(parent);
    }
    if (srcFile.isEmpty())
        return toRet;

    QSharedPointer<Importer> importer(Importer::getImporter(srcFile));
    if (importer.isNull())
        QMessageBox::warning(parent, "Error", "Import failed. Received bad importer.");
    else if (!importer->doImport())
        QMessageBox::warning(parent, "Error", "Import failed.");
    else
    {
        success = true;
        toRet = importer->results();
    }

    return toRet;
}

//static
QSharedPointer<PlanningProblem> CommonFileHandling::readProblemFromFile(QWidget *parent,
                                                                        const QString &filePath)
{
    QSharedPointer<PlanningProblem> toRet;

    QFile fp(filePath);
    if (!fp.exists())
        return toRet;
    else if (!fp.open(QFile::ReadOnly))
    {
        qWarning() << "Failed to open file" << filePath << "for reading of problem";
        QMessageBox::warning(parent, "Error", "Failed to open file " + filePath + " for reading");
        return toRet;
    }

    QDataStream stream(&fp);
    toRet = QSharedPointer<PlanningProblem>(new PlanningProblem(stream));

    return toRet;
}

//static
bool CommonFileHandling::writeChatResponseResults(UserStudyChatHandler *handler)
{
    //We only write results in "User study mode"
    if (CommonFileHandling::resultsPrefix().isEmpty())
        return true;

    bool toRet;

    const QString filename = CommonFileHandling::resultsPrefix() + " ChatResponse.csv";
    QFile fp(filename);
    if (fp.open(QFile::WriteOnly | QFile::Text))
    {
        QByteArray toWrite;
        toWrite.append(handler->toString());
        fp.write(toWrite);
        toRet = true;
    }
    else
    {
        qWarning() << "Failed to write chat response";
        toRet = false;
    }

    return toRet;
}

//static
bool CommonFileHandling::setWorkingDirectory(const QString &dirString)
{
    QFileInfo info(dirString);
    QString fullString = info.absoluteFilePath();

    if (info.exists() && !info.isDir())
    {
        qWarning() << fullString << "is not a directory";
        return false;
    }
    else if (!info.exists() && !QDir::current().mkpath(fullString))
    {
        qWarning() << "Failed to create directory" << fullString;
        return false;
    }

    qDebug() << "Working directory" << fullString;
    QDir::setCurrent(fullString);
    return true;
}

//static
void CommonFileHandling::setResultPrefix(const QString &nPrefix)
{
    CommonFileHandling::_resultPrefix = nPrefix;
}

//static
QString CommonFileHandling::resultsPrefix()
{
    return CommonFileHandling::_resultPrefix;
}
