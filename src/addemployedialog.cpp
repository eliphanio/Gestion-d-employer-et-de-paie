#include "addemployedialog.h"
#include "ui_addemployedialog.h"

addEmployeDialog::addEmployeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addEmployeDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBoxAddEmploye, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

addEmployeDialog::~addEmployeDialog()
{
    delete ui;
}
