#ifndef DIRECTIONALCONSTRAINTEDITOR_H
#define DIRECTIONALCONSTRAINTEDITOR_H

#include <QWidget>

#include "AngleRange.h"

namespace Ui {
class DirectionalConstraintEditor;
}

class DirectionalConstraintEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit DirectionalConstraintEditor(QWidget *parent = 0);
    ~DirectionalConstraintEditor();

    void setRange(const AngleRange& range);
    AngleRange range() const;
    
private:
    Ui::DirectionalConstraintEditor *ui;
};

#endif // DIRECTIONALCONSTRAINTEDITOR_H
