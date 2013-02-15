#ifndef TimingConstraintSliders_H
#define TimingConstraintSliders_H

#include <QWidget>

namespace Ui {
class TimingConstraintSliders;
}

class TimingConstraintSliders : public QWidget
{
    Q_OBJECT
    
public:
    explicit TimingConstraintSliders(QWidget *parent = 0);
    TimingConstraintSliders(int startTime, int endTime, QWidget * parent = 0);
    ~TimingConstraintSliders();

    int startTime() const;
    int endTime() const;

    void setStartTime(int nStart);
    void setEndTime(int nEnd);

signals:
    void removeRequested(TimingConstraintSliders *);
    
private slots:
    void on_startSlider_sliderMoved(int position);

    void on_endSlider_sliderMoved(int position);

    void on_removeButton_clicked();

private:
    Ui::TimingConstraintSliders *ui;
};

#endif // TimingConstraintSliders_H
