//
// mainwindow.cpp
//
// Copyright 2014 by John Pietrzak  (jpietrzak8@gmail.com)
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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "fettargetarchitecture.h"
#include <QResizeEvent>
#include "fetcamimage.h"

#ifdef ANDROID_OS
#include <QScreen>
#endif // ANDROID_OS

#include <QDebug>

MainWindow::MainWindow(
  QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindow),
    controlsHidden(true),
    webcamImage(0)
{
  ui->setupUi(this);

  // Just to be certain, set the stackedWidget index:
  ui->stackedWidget->setCurrentIndex(0);

#ifdef ANDROID_OS
  resizeIcons();
#endif // ANDROID_OS

  // Create the webcamImage widget, and push it to the backround:
  webcamImage = new FetCamImage(ui->centralWidget);
  webcamImage->lower();

  connect(
    webcamImage,
    SIGNAL(newWebcamName(QString)),
    this,
    SLOT(setWebcamName(QString)));
}


MainWindow::~MainWindow()
{
  if (webcamImage) delete webcamImage;

  delete ui;
}


void MainWindow::setWebcamName(
  QString name)
{
  ui->hWebcamLabel->setText(name);
  ui->vWebcamLabel->setText(name);
}


void MainWindow::resizeEvent(
  QResizeEvent *event)
{
  QMainWindow::resizeEvent(event);

  // Resize the image:
  webcamImage->manualResize();

  // Do nothing else if the controls are not visible:
  if (controlsHidden || (ui->stackedWidget->currentIndex() > 2)) return;

  // Switch to the proper page based on rotation:
  if (ui->centralWidget->width() > ui->centralWidget->height())
  {
    ui->stackedWidget->setCurrentIndex(1);
  }
  else
  {
    ui->stackedWidget->setCurrentIndex(2);
  }
}


void MainWindow::on_actionSelect_Webcam_triggered()
{
  webcamImage->selectWebcam();
}


void MainWindow::on_actionFilter_by_Category_triggered()
{
  webcamImage->filterByCategory();
}


void MainWindow::on_actionWebcam_Info_triggered()
{
  webcamImage->displayWebcamInfo();
}


void MainWindow::on_actionImport_Webcam_List_triggered()
{
  webcamImage->importWebcamList();
}


void MainWindow::on_actionDocumentation_triggered()
{
  ui->stackedWidget->setCurrentIndex(3);
}


void MainWindow::on_actionAbout_triggered()
{
  ui->stackedWidget->setCurrentIndex(4);
}


void MainWindow::on_fFullscreenButton_clicked()
{
  if (ui->centralWidget->width() > ui->centralWidget->height())
  {
    ui->stackedWidget->setCurrentIndex(1);
  }
  else
  {
    ui->stackedWidget->setCurrentIndex(2);
  }

  controlsHidden = false;
}


void MainWindow::on_hFullscreenButton_clicked()
{
  ui->stackedWidget->setCurrentIndex(0);

  controlsHidden = true;
}


void MainWindow::on_hNextButton_clicked()
{
  webcamImage->nextImage();
}


void MainWindow::on_hPrevButton_clicked()
{
  webcamImage->prevImage();
}


void MainWindow::on_vPrevButton_clicked()
{
  webcamImage->prevImage();
}


void MainWindow::on_vNextButton_clicked()
{
  webcamImage->nextImage();
}

void MainWindow::on_vFullscreenButton_clicked()
{
  ui->stackedWidget->setCurrentIndex(0);

  controlsHidden = true;
}

void MainWindow::on_vHomeButton_clicked()
{
  webcamImage->openHomepage();
}


void MainWindow::on_hHomeButton_clicked()
{
  webcamImage->openHomepage();
}


void MainWindow::on_hTagButton_clicked()
{
  webcamImage->filterByCategory();
}


void MainWindow::on_vTagButton_clicked()
{
  webcamImage->filterByCategory();
}


void MainWindow::on_closeAboutButton_clicked()
{
  returnToControls();
}


void MainWindow::on_closeDocButton_clicked()
{
  returnToControls();
}


void MainWindow::returnToControls()
{
  if (controlsHidden)
  {
    ui->stackedWidget->setCurrentIndex(0);
  }
  else
  {
    // Select the appropriate controls:
    if (ui->centralWidget->width() > ui->centralWidget->height())
    {
      ui->stackedWidget->setCurrentIndex(1);
    }
    else
    {
      ui->stackedWidget->setCurrentIndex(2);
    }
  }
}


#ifdef ANDROID_OS
void MainWindow::resizeIcons()
{
  QScreen *screen = qApp->primaryScreen();

  // For now, I'm guessing the average Android DPI to be around 160.
  // So, real DPI / 160 is about how much bigger the icons need to be.
  // (There's a "devicePixelRatio()" method, but it seems to be useless
  // on Android right now.)

  float pixelRatio = screen->physicalDotsPerInch() / 160;

  // I'm going to also resize the dialog boxes here:
  int iconSize;
  int dialogWidth;
  int dialogHeight;

  if (pixelRatio < 0)
  {
    // Error!  Need to send a message here.
    messenger->sendMessage("Problem determining pixel density.");
    return;
  }
  else if (pixelRatio < 1.5)
  {
    // No need to resize the icons!  Just quit.
    return;
  }
  else if (pixelRatio < 2.5)
  {
    iconSize = 96;
    dialogWidth = 1000;
    dialogHeight = 600;
  }
  else
  {
    // For now, this is as big as we will go:
    iconSize = 144;
    dialogWidth = 1500;
    dialogHeight = 900;
  }
qDebug() << "Setting iconSize to " << iconSize;

  // Now, resize all the icons.
  ui->fFullscreenButton->setIconSize(QSize(iconSize, iconSize));
  ui->hFullscreenButton->setIconSize(QSize(iconSize, iconSize));
  ui->hNextButton->setIconSize(QSize(iconSize, iconSize));
  ui->hPrevButton->setIconSize(QSize(iconSize, iconSize));
  ui->hHomeButton->setIconSize(QSize(iconSize, iconSize));
  ui->vFullscreenButton->setIconSize(QSize(iconSize, iconSize));
  ui->vNextButton->setIconSize(QSize(iconSize, iconSize));
  ui->vPrevButton->setIconSize(QSize(iconSize, iconSize));
  ui->vHomeButton->setIconSize(QSize(iconSize, iconSize));

  // Also, resize the dialogs:
  selectorDialog->resize(dialogWidth, dialogHeight);
  tagDialog->resize(dialogWidth, dialogHeight);
  infoDialog->resize(dialogWidth, dialogHeight);
}
#endif // ANDROID_OS
