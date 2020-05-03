#ifndef ADDEDITmastertoad_H
#define ADDEDITmastertoad_H

#include <QDialog>

namespace Ui {
class AddEditmastertoad;
}


class AddEditmastertoad : public QDialog
{
    Q_OBJECT

public:
    explicit AddEditmastertoad(QWidget *parent = 0);
    ~AddEditmastertoad();

protected:

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

signals:

private:
    Ui::AddEditmastertoad *ui;
};

#endif // ADDEDITmastertoad_H
