#ifndef ADDEMPLOYEDIALOG_H
#define ADDEMPLOYEDIALOG_H

#include <QDialog>

namespace Ui {
class addEmployeDialog;
}

class addEmployeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addEmployeDialog(QWidget *parent = nullptr);
    ~addEmployeDialog();

private slots:
    void on_buttonBoxAddEmploye_accepted();

private:
    Ui::addEmployeDialog *ui;
};

#endif // ADDEMPLOYEDIALOG_H
