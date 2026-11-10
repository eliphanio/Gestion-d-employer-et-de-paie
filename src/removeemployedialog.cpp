#include "removeemployedialog.h"
#include "ui_removeemployedialog.h"

removeEmployeDialog::removeEmployeDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::removeEmployeDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBoxRemoveEmploye, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

removeEmployeDialog::~removeEmployeDialog()
{
    delete ui;
}
