#include "fetaboutdialog.h"
#include "ui_fetaboutdialog.h"

FetAboutDialog::FetAboutDialog(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::FetAboutDialog)
{
  ui->setupUi(this);
}

FetAboutDialog::~FetAboutDialog()
{
  delete ui;
}
