#ifndef SAMPLINGTASKEDITORWIDGETS_H
#define SAMPLINGTASKEDITORWIDGETS_H

#include <QGroupBox>

namespace Ui {
class SamplingTaskEditorWidgets;
}

class SamplingTaskEditorWidgets : public QGroupBox
{
    Q_OBJECT
    
public:
    explicit SamplingTaskEditorWidgets(QWidget *parent = 0);
    ~SamplingTaskEditorWidgets();

    int minimumSamplingTime() const;

public slots:
    void setMinimumSamplingTime(int nTime);
    
private:
    Ui::SamplingTaskEditorWidgets *ui;
};

#endif // SAMPLINGTASKEDITORWIDGETS_H
