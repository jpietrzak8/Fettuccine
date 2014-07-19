#include "fetsslerrorsdialog.h"
#include "ui_fetsslerrorsdialog.h"

//#include <QList>
//#include <QSslError>

FetSSLErrorsDialog::FetSSLErrorsDialog(
  QWidget *parent)
  : QDialog(parent),
    ui(new Ui::FetSSLErrorsDialog),
    continueLoading(true)
{
  ui->setupUi(this);
}


FetSSLErrorsDialog::~FetSSLErrorsDialog()
{
  delete ui;
}


void FetSSLErrorsDialog::setErrors(
  QList<QSslError> errors)
{
  int i = 0;
  int size = errors.count();
  QString errorsText;

  while (i < size)
  {
    if (i > 0) errorsText += "\n";
    errorsText += errors[i].errorString();
    ++i;
  }

  ui->errorBrowser->setText(errorsText);
}


void FetSSLErrorsDialog::on_buttonBox_accepted()
{
  continueLoading = true;
}


void FetSSLErrorsDialog::on_buttonBox_rejected()
{
  continueLoading = false;
}
