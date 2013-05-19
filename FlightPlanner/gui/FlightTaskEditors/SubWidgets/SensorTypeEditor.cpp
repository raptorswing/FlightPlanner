#include "SensorTypeEditor.h"
#include "ui_SensorTypeEditor.h"

const int INDEX_DIRECTIONAL = 0;
const int INDEX_OMNIDIRECTIONAL = 1;

SensorTypeEditor::SensorTypeEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SensorTypeEditor)
{
    ui->setupUi(this);

    this->ui->sensorTypeCombo->addItem("Directional");
    this->ui->sensorTypeCombo->addItem("Omnidirectional");
}

SensorTypeEditor::~SensorTypeEditor()
{
    delete ui;
}

FlightTask::SensorType SensorTypeEditor::sensorType() const
{
    if (this->ui->sensorTypeCombo->currentIndex() == INDEX_DIRECTIONAL)
        return FlightTask::DirectionalSensor;
    else
        return FlightTask::OmnidirectionalSensor;
}

QString SensorTypeEditor::sensorTypeString() const
{
    return FlightTask::sensorType2String(this->sensorType());
}


void SensorTypeEditor::setSensorType(FlightTask::SensorType sType)
{
    if (sType == FlightTask::DirectionalSensor)
        this->ui->sensorTypeCombo->setCurrentIndex(INDEX_DIRECTIONAL);
    else
        this->ui->sensorTypeCombo->setCurrentIndex(INDEX_OMNIDIRECTIONAL);
}

void SensorTypeEditor::setSensorTypeString(const QString &string)
{
    this->setSensorType(FlightTask::string2SensorType(string));
}
