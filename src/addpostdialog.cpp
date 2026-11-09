#include "addpostdialog.h"
#include "ui_addpostdialog.h"

addPostDialog::addPostDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addPostDialog)
{
    ui->setupUi(this);
    connect(ui->buttonBoxAddPoste, &QDialogButtonBox::rejected,
            this, &QDialog::reject);
}

addPostDialog::~addPostDialog()
{
    delete ui;
}
