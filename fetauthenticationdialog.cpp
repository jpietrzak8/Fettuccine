#include "fetauthenticationdialog.h"
#include "ui_fetauthenticationdialog.h"

FetAuthenticationDialog::FetAuthenticationDialog(
  QWidget *parent)
  : QDialog(parent),
    ui(new Ui::FetAuthenticationDialog),
    accepted(false)
{
  ui->setupUi(this);
}

FetAuthenticationDialog::~FetAuthenticationDialog()
{
  delete ui;
}

QString FetAuthenticationDialog::getUser()
{
  return ui->userEdit->text();
}

QString FetAuthenticationDialog::getPassword()
{
  return ui->passwordEdit->text();
}

void FetAuthenticationDialog::on_buttonBox_accepted()
{
  accepted = true;
}

void FetAuthenticationDialog::on_buttonBox_rejected()
{
  accepted = false;
}

void FetAuthenticationDialog::on_showPasswordCheckBox_toggled(bool checked)
{
  if (checked)
  {
    ui->passwordEdit->setEchoMode(QLineEdit::Normal);
  }
  else
  {
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
  }
}
