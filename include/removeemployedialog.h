#ifndef REMOVEEMPLOYEDIALOG_H
#define REMOVEEMPLOYEDIALOG_H

#include <QDialog>

namespace Ui {
class removeEmployeDialog;
}

class removeEmployeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit removeEmployeDialog(QWidget *parent = nullptr);
    ~removeEmployeDialog();

private:
    Ui::removeEmployeDialog *ui;
};

#endif // REMOVEEMPLOYEDIALOG_H
