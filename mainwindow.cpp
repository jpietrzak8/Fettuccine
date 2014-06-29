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
#include "fetcamselectordialog.h"
#include "fettagdialog.h"
#include "fetcaminfodialog.h"
#include "fetcamwidgetitem.h"
#include <QTimer>
#include <QUrl>
#include <QNetworkReply>
#include <QFile>
#include <QXmlStreamReader>
#include <QList>
#include <QSettings>
#include <QFileDialog>
#include <QResizeEvent>
#include <QDesktopServices>

#ifdef ANDROID_OS
#include <QScreen>
#endif // ANDROID_OS

#include <QDebug>

MainWindow::MainWindow(
  QWidget *parent)
  : QMainWindow(parent),
    ui(new Ui::MainWindow),
    selectorDialog(0),
    tagDialog(0),
    infoDialog(0),
    messenger(0),
    timer(0),
    controlsHidden(true),
    webcamImage(0)
{
  ui->setupUi(this);

#ifdef ANDROID_OS
  resizeIcons();
#endif // ANDROID_OS

  // Create the webcamImage widget, and push it to the backround:
  webcamImage = new FetCamImage(ui->centralWidget);
  webcamImage->lower();

  selectorDialog = new FetCamSelectorDialog(this);
  tagDialog = new FetTagDialog(this);
  infoDialog = new FetCamInfoDialog(this);
  messenger = new FetMessenger(this);

  connect(
    webcamImage,
    SIGNAL(imageError(QString)),
    messenger,
    SLOT(sendMessage(QString)));

  connect(
    selectorDialog,
    SIGNAL(addNewTag(QString)),
    tagDialog,
    SLOT(addTag(QString)));

  connect(
    selectorDialog,
    SIGNAL(showWebcam(FetCamWidgetItem *)),
    this,
    SLOT(loadWebcam(FetCamWidgetItem *)));

  connect(
    selectorDialog,
    SIGNAL(selectorWarning(QString)),
    messenger,
    SLOT(sendMessage(QString)));

  connect(
    tagDialog,
    SIGNAL(selectByTag(QString)),
    selectorDialog,
    SLOT(filterList(QString)));

  connect(
    &qnam,
    SIGNAL(finished(QNetworkReply *)),
    this,
    SLOT(updateWidget(QNetworkReply *)));

  timer = new QTimer(this);

  connect(
    timer,
    SIGNAL(timeout()),
    this,
    SLOT(retrieveImage()));

  // Need to populate the list after the connections have been made:
  selectorDialog->populateList();

  QSettings settings("pietrzak.org", "Fettuccine");

  if (settings.contains("currentWebcamID"))
  {
    selectorDialog->chooseItem(
      settings.value("currentWebcamID").toInt());
  }
  else
  {
    // Start with the first item on the list:
    selectorDialog->chooseItem(0);
  }
}


MainWindow::~MainWindow()
{
  QSettings settings("pietrzak.org", "Fettuccine");
 
  settings.setValue("currentWebcamID", selectorDialog->getCurrentID());

  if (timer) delete timer;
  if (messenger) delete messenger;
  if (infoDialog) delete infoDialog;
  if (tagDialog) delete tagDialog;
  if (selectorDialog) delete selectorDialog;

  delete ui;
}


void MainWindow::resizeEvent(
  QResizeEvent *event)
{
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

  event->accept();
}


void MainWindow::retrieveImage()
{
  qnam.get(QNetworkRequest(QUrl(currentWebcamUrl)));
}


void MainWindow::updateWidget(
  QNetworkReply *reply)
{
  // Check this reply against the current webcam URL:
  if (reply->request().url() != QUrl(currentWebcamUrl))
  {
qDebug() << "reply->request().url().toString() " << reply->request().url().toString();
qDebug() << "currentWebcamUrl " << currentWebcamUrl;
    // This reply is for an earlier webcam, so don't bother with it.
    return;
  }

  // Support some amount of redirection here:
  QUrl redirectUrl =
    reply->attribute(
      QNetworkRequest::RedirectionTargetAttribute).toUrl();

  if (!redirectUrl.isEmpty())
  {
    currentWebcamUrl = redirectUrl.toString();
    qnam.get(QNetworkRequest(redirectUrl));
    reply->deleteLater();
    return;
  }

  // We should have an image at this point.
  webcamImage->setImage(reply->readAll());

  reply->deleteLater();
}


void MainWindow::loadWebcam(
  FetCamWidgetItem *item)
{
  currentWebcamUrl = item->getLink();
  currentWebcamHomepage = item->getHomepage();
  ui->hWebcamLabel->setText(item->getName());
  ui->vWebcamLabel->setText(item->getName());

  retrieveImage();

  timer->start(item->getRefreshRate() * 1000);
}


void MainWindow::on_actionSelect_Webcam_triggered()
{
#ifdef ANDROID_OS
  selectorDialog->reposition(); // needed for Qt/Android
#endif // ANDROID_OS
  selectorDialog->exec();
}


void MainWindow::on_actionFilter_by_Category_triggered()
{
  tagDialog->exec();
}


void MainWindow::on_actionWebcam_Info_triggered()
{
  infoDialog->setupDialog(
    selectorDialog->getCurrentItem());

#ifdef ANDROID_OS
  infoDialog->reposition();  // Needed for Qt/Android
#endif // ANDROID_OS
  infoDialog->exec();
}


void MainWindow::on_actionImport_Webcam_List_triggered()
{
  // Ask the user to select the xml file:
  QString filename = QFileDialog::getOpenFileName(
    this, "Select Webcam XML File");

  if (filename.isEmpty())
  {
    // The user did not choose a file.
    return;
  }

  QFile webcamFile(filename);

  if (!webcamFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString errString;
    errString += "Unable to open ";
    errString += filename;
    messenger->sendMessage(errString);
qDebug() << "Unable to open " << filename;
    return;
  }

  QXmlStreamReader webcamReader(&webcamFile);

  QList<FetCamWidgetItem *> camList;

  parseWebcamXml(webcamReader, camList);

  if (camList.isEmpty())
  {
    QString errString;
    errString += "No webcams found in ";
    errString += filename;
    messenger->sendMessage(errString);
qDebug() << "No Webcams Found in " << filename;;
    return;
  }

  // Empty out the current list:
  selectorDialog->clearWebcams();

  int i = 0;
  int size = camList.size();
  while (i < size)
  {
    selectorDialog->insertWebcam(camList[i]);
    ++i;
  }

  loadWebcam(camList[0]);
}


void MainWindow::on_actionDocumentation_triggered()
{
  ui->stackedWidget->setCurrentIndex(3);
}


void MainWindow::on_actionAbout_triggered()
{
  ui->stackedWidget->setCurrentIndex(4);
}


void MainWindow::parseWebcamXml(
  QXmlStreamReader &webcamReader,
  QList<FetCamWidgetItem *> &camList)
{
  while(!webcamReader.atEnd())
  {
    webcamReader.readNext();

    if (webcamReader.isStartElement())
    {
      if (webcamReader.name() == "fettuccine")
      {
//qDebug() << "found fettuccine element";
        parseFettuccineElement(webcamReader, camList);
      }
    }
  }

  if (webcamReader.hasError())
  {
    QString err;
    err.append("QXmlStreamReader returned error: ");
    err.append(webcamReader.errorString());
    messenger->sendMessage(err);
//qDebug() << err;
    return;
  }
}

void MainWindow::parseFettuccineElement(
  QXmlStreamReader &webcamReader,
  QList<FetCamWidgetItem *> &camList)
{
  QString link;
  QString homepage;
  int refreshRate = 30; // default to 30 seconds.

  while (!webcamReader.atEnd())
  {
    webcamReader.readNext();

    if (webcamReader.isStartElement())
    {
      if (webcamReader.name() == "webcam")
      {
        if (webcamReader.attributes().hasAttribute("link"))
        {
          link = webcamReader.attributes().value("link").toString();
        }

        if (webcamReader.attributes().hasAttribute("homepage"))
        {
          homepage = webcamReader.attributes().value("homepage").toString();
        }

        if (webcamReader.attributes().hasAttribute("refreshRate"))
        {
          // Qt 4.8 needs conversion to string to get int, Qt 5.0 doesn not.
#ifdef MAEMO_OS
          refreshRate = webcamReader.attributes().value("refreshRate").toString().toInt();
#else
          refreshRate = webcamReader.attributes().value("refreshRate").toInt();
#endif // MAEMO_OS
        }

        FetCamWidgetItem *item =
          new FetCamWidgetItem(link, homepage, refreshRate);

        parseWebcamElement(webcamReader, item);

        camList.append(item);

        link.clear();
        homepage.clear();
        refreshRate = 30;
      }
    }

    else if (webcamReader.isEndElement())
    {
      if (webcamReader.name() == "fettuccine")
      {
        break;
      }
    }
  }
}


void MainWindow::parseWebcamElement(
  QXmlStreamReader &webcamReader,
  FetCamWidgetItem *item)
{
  QString textString;

  while (!webcamReader.atEnd())
  {
    webcamReader.readNext();

    if (webcamReader.isStartElement())
    {
      if (webcamReader.name() == "name")
      {
        textString = parseText(webcamReader, "name");
        if (!textString.isEmpty())
        {
          item->setName(textString);
        }
      }
      else if (webcamReader.name() == "tag")
      {
        textString = parseText(webcamReader, "tag");
        if (!textString.isEmpty())
        {
          item->addTag(textString);
          tagDialog->addTag(textString);
        }
      }
    }
    else if (webcamReader.isEndElement())
    {
      if (webcamReader.name() == "webcam")
      {
        break;
      }
    }
  }
}


QString MainWindow::parseText(
  QXmlStreamReader &webcamReader,
  QString elementName)
{
  QString textString;

  while (!webcamReader.atEnd())
  {
    webcamReader.readNext();

    if (webcamReader.isCharacters() && !webcamReader.isWhitespace())
    {
      textString.append(webcamReader.text().toString());
    }
    else if (webcamReader.isEndElement())
    {
      if (webcamReader.name() == elementName)
      {
        break;
      }
    }
  }

  return textString;
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
  selectorDialog->chooseNextItem();
}

void MainWindow::on_hPrevButton_clicked()
{
  selectorDialog->choosePreviousItem();
}

void MainWindow::on_vPrevButton_clicked()
{
  selectorDialog->choosePreviousItem();
}

void MainWindow::on_vNextButton_clicked()
{
  selectorDialog->chooseNextItem();
}

void MainWindow::on_vFullscreenButton_clicked()
{
  ui->stackedWidget->setCurrentIndex(0);

  controlsHidden = true;
}

void MainWindow::on_vHomeButton_clicked()
{
  QDesktopServices::openUrl(QUrl(currentWebcamHomepage));
}

void MainWindow::on_hHomeButton_clicked()
{
  QDesktopServices::openUrl(QUrl(currentWebcamHomepage));
}

void MainWindow::on_hTagButton_clicked()
{
  tagDialog->exec();
}

void MainWindow::on_vTagButton_clicked()
{
  tagDialog->exec();
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
