#ifndef mastertoadCONFIGDIALOG_H
#define mastertoadCONFIGDIALOG_H

#include <QDialog>

namespace Ui {
    class mastertoadConfigDialog;
}

QT_BEGIN_NAMESPACE
class QModelIndex;
QT_END_NAMESPACE

/** Dialog showing transaction details. */
class mastertoadConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit mastertoadConfigDialog(QWidget *parent = 0, QString nodeAddress = "123.456.789.123:9999", QString privkey="MASTERNODEPRIVKEY");
    ~mastertoadConfigDialog();

private:
    Ui::mastertoadConfigDialog *ui;
};

#endif // mastertoadCONFIGDIALOG_H
