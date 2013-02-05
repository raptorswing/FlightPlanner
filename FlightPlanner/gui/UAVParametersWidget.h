#ifndef UAVPARAMETERSWIDGET_H
#define UAVPARAMETERSWIDGET_H

#include <QWidget>

namespace Ui {
class UAVParametersWidget;
}

class UAVParametersWidget : public QWidget
{
    Q_OBJECT
public:
    explicit UAVParametersWidget(QWidget *parent = 0);
    ~UAVParametersWidget();
    
signals:
    
public slots:

private:
    Ui::UAVParametersWidget *ui;
    
};

#endif // UAVPARAMETERSWIDGET_H
