#ifndef SENSORTYPEEDITOR_H
#define SENSORTYPEEDITOR_H

#include <QWidget>

#include "FlightTasks/FlightTask.h"

namespace Ui {
class SensorTypeEditor;
}

class SensorTypeEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit SensorTypeEditor(QWidget *parent = 0);
    ~SensorTypeEditor();

    FlightTask::SensorType sensorType() const;
    QString sensorTypeString() const;

    void setSensorType(FlightTask::SensorType sType);
    void setSensorTypeString(const QString& string);

    
private:
    Ui::SensorTypeEditor *ui;
};

#endif // SENSORTYPEEDITOR_H
