#include "fetwidgetsettingsdialog.h"
#include "ui_fetwidgetsettingsdialog.h"

#include <QListWidgetItem>
#include "fetaboutdialog.h"


FetWidgetSettingsDialog::FetWidgetSettingsDialog(
  QWidget *parent)
  : QDialog(parent),
    ui(new Ui::FetWidgetSettingsDialog),
    aboutDialog(0)
{
  ui->setupUi(this);

  aboutDialog = new FetAboutDialog();
}

FetWidgetSettingsDialog::~FetWidgetSettingsDialog()
{
  if (aboutDialog) delete aboutDialog;

  delete ui;
}

void FetWidgetSettingsDialog::on_listWidget_itemClicked(QListWidgetItem *item)
{
  QString text = item->text();

  // This could be done better:
  if (text == "Select Webcam")
  {
    emit selectWebcam();
  }
  else if (text == "Filter by Category")
  {
    emit filterByCategory();
  }
  else if (text == "Webcam Info")
  {
    emit displayWebcamInfo();
  }
  else if (text == "Import Webcam List")
  {
    emit importWebcamList();
  }
  else if (text == "About")
  {
    aboutDialog->exec();
  }
}

void FetWidgetSettingsDialog::on_buttonBox_accepted()
{
  emit newSize(
    ui->widthSpinBox->value(),
    ui->heightSpinBox->value());
}
