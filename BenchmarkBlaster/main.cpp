#include <QCoreApplication>

#include "BenchmarkDriver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    BenchmarkDriver driver;
    
    return a.exec();
}
