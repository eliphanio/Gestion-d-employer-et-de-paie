#ifndef UPDATEPOSTEDIALOG_H
#define UPDATEPOSTEDIALOG_H

#include <QDialog>

namespace Ui {
class updatePosteDialog;
}

class updatePosteDialog : public QDialog
{
    Q_OBJECT

public:
    explicit updatePosteDialog(QWidget *parent = nullptr);
    ~updatePosteDialog();
    void rechercherPoste();

private slots:
    void on_buttonBoxUpdatePoste_accepted();

private:
    Ui::updatePosteDialog *ui;
};

#endif // UPDATEPOSTEDIALOG_H
