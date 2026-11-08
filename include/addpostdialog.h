#ifndef ADDPOSTDIALOG_H
#define ADDPOSTDIALOG_H

#include <QDialog>

namespace Ui {
class addPostDialog;
}

class addPostDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addPostDialog(QWidget *parent = nullptr);
    ~addPostDialog();

private:
    Ui::addPostDialog *ui;
};

#endif // ADDPOSTDIALOG_H
