#include <QApplication>
#include "gui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow * w = new MainWindow();
    w->show();
    w->setAttribute(Qt::WA_DeleteOnClose);
    
    return a.exec();
}
