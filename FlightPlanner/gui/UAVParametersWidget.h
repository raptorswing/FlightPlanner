#ifndef UAVPARAMETERSWIDGET_H
#define UAVPARAMETERSWIDGET_H

#include "UAVParameters.h"

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

    void setParameters(const UAVParameters& params);
    UAVParameters parameters() const;
    
signals:
    
public slots:

private:
    Ui::UAVParametersWidget *ui;
    
};

#endif // UAVPARAMETERSWIDGET_H
