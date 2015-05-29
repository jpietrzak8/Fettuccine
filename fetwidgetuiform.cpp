//
// fetwidgetuiform.cpp
//
// Copyright 2014-2015 by John Pietrzak  (jpietrzak8@gmail.com)
//
// This file is part of Fettuccine.
//
// Fettuccine is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// Fettuccine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Fettuccine; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

#include "fetwidgetuiform.h"
#include "ui_fetwidgetuiform.h"

#include "fetcamimage.h"
#include "fetwidgetsettingsdialog.h"
#include <QResizeEvent>
#include <QFontMetrics>
#include <QSettings>


FetWidgetUIForm::FetWidgetUIForm(
  QWidget *parent)
  : QWidget(parent),
    ui(new Ui::FetWidgetUIForm),
    webcamImage(0),
    settingsDialog(0),
    width(320),
    height(200)
{
  ui->setupUi(this);

  // To start with, hide the UI:
  ui->nextButton->hide();
  ui->prevButton->hide();
  ui->nameLabel->hide();
  ui->statusButton->hide();

  QSettings settings("pietrzak.org", "Fettuccine");

  if (settings.contains("WidgetWidth"))
  {
    width = settings.value("WidgetWidth").toInt();
  }

  if (settings.contains("WidgetHeight"))
  {
    height = settings.value("WidgetHeight").toInt();
  }

  // Ridiculous backwards parenting method to get around Maemo Widget bug:
  webcamImage = new FetCamImage();
  webcamImage->setParent(this);

  webcamImage->lower();

  connect(
    webcamImage,
    SIGNAL(newWebcamName(QString)),
    this,
    SLOT(setWebcamName(QString)));

  connect(
    webcamImage,
    SIGNAL(pauseDisplay()),
    this,
    SLOT(pauseWidget()));

  webcamImage->loadFirstImage();
}


FetWidgetUIForm::~FetWidgetUIForm()
{
  QSettings settings("pietrzak.org", "Fettuccine");

  settings.setValue("WidgetWidth", width);
  settings.setValue("WidgetHeight", height);

  if (webcamImage) delete webcamImage;
  if (settingsDialog) delete settingsDialog;

  delete ui;
}


QSize FetWidgetUIForm::sizeHint() const
{
  return QSize(width, height);
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


void FetWidgetUIForm::on_statusButton_clicked()
{
  webcamImage->exitSleepMode();

  ui->statusButton->hide();
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


void FetWidgetUIForm::pauseWidget()
{
  webcamImage->enterSleepMode();

  ui->statusButton->show();
}
