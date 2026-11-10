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

private slots:
    void on_buttonBoxRemoveEmploye_accepted();

private:
    Ui::removeEmployeDialog *ui;
};

#endif // REMOVEEMPLOYEDIALOG_H
