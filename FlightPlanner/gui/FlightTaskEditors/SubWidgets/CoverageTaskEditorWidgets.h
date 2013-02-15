#ifndef COVERAGETASKEDITORWIDGETS_H
#define COVERAGETASKEDITORWIDGETS_H

#include <QWidget>

namespace Ui {
class CoverageTaskEditorWidgets;
}

class CoverageTaskEditorWidgets : public QWidget
{
    Q_OBJECT
    
public:
    explicit CoverageTaskEditorWidgets(QWidget *parent = 0);
    ~CoverageTaskEditorWidgets();

    qreal granularity() const;
    qreal maxDistance() const;

public slots:
    void setGranularity(qreal nGran);
    void setMaxDistance(qreal nDist);
    
private:
    Ui::CoverageTaskEditorWidgets *ui;
};

#endif // COVERAGETASKEDITORWIDGETS_H
