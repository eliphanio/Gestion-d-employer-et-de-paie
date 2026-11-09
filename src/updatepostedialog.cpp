#include "updatepostedialog.h"
#include "ui_updatepostedialog.h"

updatePosteDialog::updatePosteDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::updatePosteDialog)
{
    ui->setupUi(this);
}

updatePosteDialog::~updatePosteDialog()
{
    delete ui;
}
