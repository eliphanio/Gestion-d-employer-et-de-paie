#include "updateemployedialog.h"
#include "ui_updateemployedialog.h"

updateEmployeDialog::updateEmployeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::updateEmployeDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBoxUpdateEmploye, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

updateEmployeDialog::~updateEmployeDialog()
{
    delete ui;
}
