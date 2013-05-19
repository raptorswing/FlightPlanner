#ifndef MINSENSINGDISTEDITOR_H
#define MINSENSINGDISTEDITOR_H

#include <QWidget>

namespace Ui {
class MinSensingDistEditor;
}

class MinSensingDistEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit MinSensingDistEditor(QWidget *parent = 0);
    ~MinSensingDistEditor();

    qreal min() const;
    qreal max() const;

    void setRange(qreal min, qreal max);
    
private slots:
    void on_minDistSpinbox_valueChanged(double arg1);
    void on_maxDistSpinbox_valueChanged(double arg1);

private:
    Ui::MinSensingDistEditor *ui;
};

#endif // MINSENSINGDISTEDITOR_H
