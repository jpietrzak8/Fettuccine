#include "fetwidgetuiform.h"
#include "ui_fetwidgetuiform.h"

#include "fetcamimage.h"
#include "fetwidgetsettingsdialog.h"
#include <QResizeEvent>
#include <QFontMetrics>


FetWidgetUIForm::FetWidgetUIForm(
  QWidget *parent)
  : QWidget(parent),
    ui(new Ui::FetWidgetUIForm),
    webcamImage(0),
    settingsDialog(0)
{
  ui->setupUi(this);

  // To start with, hide the UI:
  ui->nextButton->hide();
  ui->prevButton->hide();
  ui->nameLabel->hide();

  // Ridiculous backwards parenting method to get around Maemo Widget bug:
  webcamImage = new FetCamImage();
  webcamImage->setParent(this);

  webcamImage->lower();

  connect(
    webcamImage,
    SIGNAL(newWebcamName(QString)),
    this,
    SLOT(setWebcamName(QString)));

  webcamImage->loadFirstImage();
}


FetWidgetUIForm::~FetWidgetUIForm()
{
  if (webcamImage) delete webcamImage;
  if (settingsDialog) delete settingsDialog;

  delete ui;
}


QSize FetWidgetUIForm::sizeHint() const
{
  return QSize(320, 200);
}


void FetWidgetUIForm::setupSettingsDialog()
{
  // Apparently, widget dialogs must not be parented?
  settingsDialog = new FetWidgetSettingsDialog();

  connect(
    settingsDialog,
    SIGNAL(selectWebcam()),
    webcamImage,
    SLOT(selectWebcam()));

  connect(
    settingsDialog,
    SIGNAL(filterByCategory()),
    webcamImage,
    SLOT(filterByCategory()));

  connect(
    settingsDialog,
    SIGNAL(displayWebcamInfo()),
    webcamImage,
    SLOT(displayWebcamInfo()));

  connect(
    settingsDialog,
    SIGNAL(importWebcamList()),
    webcamImage,
    SLOT(importWebcamList()));

  connect(
    settingsDialog,
    SIGNAL(newSize(int, int)),
    this,
    SLOT(changeSizeTo(int, int)));

  connect(
    settingsDialog,
    SIGNAL(enableNavigation(bool)),
    this,
    SLOT(displayNavButtons(bool)));

  connect(
    settingsDialog,
    SIGNAL(enableTitle(bool)),
    this,
    SLOT(displayTitle(bool)));
}


void FetWidgetUIForm::setWebcamName(
  QString name)
{
  // Need to force this text to fit within the existing bounds:
  QFontMetrics metrics(ui->nameLabel->font());
  ui->nameLabel->setText(
    metrics.elidedText(name, Qt::ElideRight, ui->nameLabel->width()));
}


void FetWidgetUIForm::on_nextButton_clicked()
{
  webcamImage->nextImage();
}


void FetWidgetUIForm::on_prevButton_clicked()
{
  webcamImage->prevImage();
}


void FetWidgetUIForm::showSettingsDialog()
{
  settingsDialog->exec();
}


void FetWidgetUIForm::resizeEvent(
  QResizeEvent *event)
{
  QWidget::resizeEvent(event);

  webcamImage->manualResize();
}


void FetWidgetUIForm::changeSizeTo(
  int width,
  int height)
{
  // Hopefully, this will be enough:
  resize(width, height);
}


void FetWidgetUIForm::displayNavButtons(
  bool showButtons)
{
  if (showButtons)
  {
    ui->nextButton->show();
    ui->prevButton->show();
  }
  else
  {
    ui->nextButton->hide();
    ui->prevButton->hide();
  }
}


void FetWidgetUIForm::displayTitle(
  bool showTitle)
{
  if (showTitle)
  {
    ui->nameLabel->show();
  }
  else
  {
    ui->nameLabel->hide();
  }
}
