#ifndef TASKNAMEEDITOR_H
#define TASKNAMEEDITOR_H

#include <QWidget>

namespace Ui {
class TaskNameEditor;
}

class TaskNameEditor : public QWidget
{
    Q_OBJECT
    
public:
    explicit TaskNameEditor(QWidget *parent = 0);
    ~TaskNameEditor();

    QString name() const;

public slots:
    void setName(const QString& nName);
    
private:
    Ui::TaskNameEditor *ui;
};

#endif // TASKNAMEEDITOR_H
