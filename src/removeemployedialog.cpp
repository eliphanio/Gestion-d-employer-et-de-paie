#include "removeemployedialog.h"
#include "ui_removeemployedialog.h"

removeEmployeDialog::removeEmployeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::removeEmployeDialog)
{
    ui->setupUi(this);
}

removeEmployeDialog::~removeEmployeDialog()
{
    delete ui;
}
