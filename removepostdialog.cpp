#include "removepostdialog.h"
#include "ui_removepostdialog.h"

removePostDialog::removePostDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::removePostDialog)
{
    ui->setupUi(this);
}

removePostDialog::~removePostDialog()
{
    delete ui;
}
