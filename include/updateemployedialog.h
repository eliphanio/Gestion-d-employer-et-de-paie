#ifndef UPDATEEMPLOYEDIALOG_H
#define UPDATEEMPLOYEDIALOG_H

#include <QDialog>

namespace Ui {
class updateEmployeDialog;
}

class updateEmployeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit updateEmployeDialog(QWidget *parent = nullptr);
    ~updateEmployeDialog();

private:
    Ui::updateEmployeDialog *ui;
};

#endif // UPDATEEMPLOYEDIALOG_H
