#include "updatepostedialog.h"
#include "ui_updatepostedialog.h"

updatePosteDialog::updatePosteDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::updatePosteDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBoxUpdatePoste, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

updatePosteDialog::~updatePosteDialog()
{
    delete ui;
}
