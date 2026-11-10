#ifndef REMOVEPOSTDIALOG_H
#define REMOVEPOSTDIALOG_H

#include <QDialog>

namespace Ui {
class removePostDialog;
}

class removePostDialog : public QDialog
{
    Q_OBJECT

public:
    explicit removePostDialog(QWidget *parent = nullptr);
    ~removePostDialog();

private slots:
    void on_buttonBoxRemovePoste_accepted();

private:
    Ui::removePostDialog *ui;
};

#endif // REMOVEPOSTDIALOG_H
