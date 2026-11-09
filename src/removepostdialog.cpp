#include "removepostdialog.h"
#include "ui_removepostdialog.h"

removePostDialog::removePostDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::removePostDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBoxRemovePoste, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

removePostDialog::~removePostDialog()
{
    delete ui;
}
