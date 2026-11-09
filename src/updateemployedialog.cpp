#include "updateemployedialog.h"
#include "ui_updateemployedialog.h"

updateEmployeDialog::updateEmployeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::updateEmployeDialog)
{
    ui->setupUi(this);
}

updateEmployeDialog::~updateEmployeDialog()
{
    delete ui;
}
