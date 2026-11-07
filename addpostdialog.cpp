#include "addpostdialog.h"
#include "ui_addpostdialog.h"

addPostDialog::addPostDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addPostDialog)
{
    ui->setupUi(this);
}

addPostDialog::~addPostDialog()
{
    delete ui;
}
