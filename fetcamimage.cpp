//
// fetcamimage.cpp
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

#include <fetcamimage.h>

#include <QByteArray>
#include <QMovie>
#include <QBuffer>
#include <QSize>
#include <QResizeEvent>
#include "fettargetarchitecture.h"
#include "fetcamselectordialog.h"
#include "fettagdialog.h"
#include "fetcaminfodialog.h"
#include "fetloadfiledialog.h"
#include "fetcamwidgetitem.h"
#include "fetwidgetsettingsdialog.h"
#include "fetmessenger.h"
#include <QTimer>
#include <QUrl>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QSettings>

#include <QDebug>

FetCamImage::FetCamImage(
  QWidget *parent)
  : QLabel("Loading Webcam", parent),
    imageData(0),
    imageBuffer(0),
    currentMovie(0),
    selectorDialog(0),
    tagDialog(0),
    infoDialog(0),
    loadFileDialog(0),
    settingsDialog(0),
    messenger(0),
    currentReply(0),
    timer(0),
    numberOfRedirections(0)
{
  setAlignment(Qt::AlignCenter);

  selectorDialog = new FetCamSelectorDialog(parent);
  tagDialog = new FetTagDialog(parent);
  infoDialog = new FetCamInfoDialog(parent);
  loadFileDialog = new FetLoadFileDialog(parent);
  messenger = new FetMessenger(parent);
  timer = new QTimer();

  connect(
    selectorDialog,
    SIGNAL(addNewTag(QString)),
    tagDialog,
    SLOT(addTag(QString)));

  connect(
    selectorDialog,
    SIGNAL(clearTags()),
    tagDialog,
    SLOT(clearTags()));

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
    loadFileDialog,
    SIGNAL(newCamList(FetCamCollection)),
    selectorDialog,
    SLOT(loadWebcams(FetCamCollection)));

  connect(
    loadFileDialog,
    SIGNAL(loadFileError(QString)),
    messenger,
    SLOT(sendMessage(QString)));

  connect(
    &qnam,
    SIGNAL(finished(QNetworkReply *)),
    this,
    SLOT(updateWidget(QNetworkReply *)));

  connect(
    timer,
    SIGNAL(timeout()),
    this,
    SLOT(retrieveImage()));

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


FetCamImage::~FetCamImage()
{
  QSettings settings("pietrzak.org", "Fettuccine");
 
  settings.setValue("currentWebcamID", selectorDialog->getCurrentID());

  if (timer) delete timer;
  if (messenger) delete messenger;
  if (settingsDialog) delete settingsDialog;
  if (loadFileDialog) delete loadFileDialog;
  if (infoDialog) delete infoDialog;
  if (tagDialog) delete tagDialog;
  if (selectorDialog) delete selectorDialog;
  if (currentMovie) delete currentMovie;
  if (imageBuffer) delete imageBuffer;
  if (imageData) delete imageData;
}


QSize FetCamImage::sizeHint() const
{
  return QSize(320, 200);
}


void FetCamImage::setupSettingsDialog()
{
  // Apparently, widget dialogs must not be parented?
  settingsDialog = new FetWidgetSettingsDialog();

  connect(
    settingsDialog,
    SIGNAL(selectWebcam()),
    this,
    SLOT(selectWebcam()));

  connect(
    settingsDialog,
    SIGNAL(filterByCategory()),
    this,
    SLOT(filterByCategory()));

  connect(
    settingsDialog,
    SIGNAL(displayWebcamInfo()),
    this,
    SLOT(displayWebcamInfo()));

  connect(
    settingsDialog,
    SIGNAL(importWebcamList()),
    this,
    SLOT(importWebcamList()));

  connect(
    settingsDialog,
    SIGNAL(newSize(int, int)),
    this,
    SLOT(changeSizeTo(int, int)));
}


void FetCamImage::changeSizeTo(
  int width,
  int height)
{
  // Hopefully, this will be enough:
  resize(width, height);
}


void FetCamImage::retrieveImage()
{
  if (currentReply)
  {
    currentReply->abort();
    currentReply->deleteLater();
  }

  currentReply = qnam.get(QNetworkRequest(QUrl(currentWebcamUrl)));
}


void FetCamImage::updateWidget(
  QNetworkReply *reply)
{
  // Check this reply against the current webcam URL:
  if (reply->request().url() != QUrl(currentWebcamUrl))
  {
//qDebug() << "reply->request().url().toString() " << reply->request().url().toString();
//qDebug() << "currentWebcamUrl " << currentWebcamUrl;
    // This reply is for an earlier webcam, so don't bother with it.

    reply->deleteLater();
    return;
  }

  // Support some amount of redirection here:
  QUrl redirectUrl =
    reply->attribute(
      QNetworkRequest::RedirectionTargetAttribute).toUrl();

  if (!redirectUrl.isEmpty())
  {
    // Avoid infinite loop:
    if (numberOfRedirections > 10)
    {
      messenger->sendMessage("Too many URL redirections, cannot load image.");
      // reset the counter:
      numberOfRedirections = 0;
      return;
    }
    else
    {
      ++numberOfRedirections;
    }

    currentWebcamUrl = redirectUrl.toString();
    currentReply = qnam.get(QNetworkRequest(redirectUrl));
    reply->deleteLater();
    return;
  }
  else
  {
    // Reset the counter:
    numberOfRedirections = 0;
  }

  // We should have an image at this point.
  setImage(reply->readAll());

  reply->deleteLater();
  currentReply = 0; // this is a hack
}


// This will probably only be used on Maemo, right?
void FetCamImage::showSettingsDialog()
{
  settingsDialog->exec();
}


void FetCamImage::selectWebcam()
{
#ifdef ANDROID_OS
  selectorDialog->reposition(); // needed for Qt/Android
#endif // ANDROID_OS
  selectorDialog->exec();
}


void FetCamImage::filterByCategory()
{
#ifdef ANDROID_OS
  tagDialog->reposition(); // needed for Qt/Android
#endif // ANDROID_OS
  tagDialog->exec();
}


void FetCamImage::displayWebcamInfo()
{
  infoDialog->setupDialog(
    selectorDialog->getCurrentItem());

#ifdef ANDROID_OS
  infoDialog->reposition();  // Needed for Qt/Android
#endif // ANDROID_OS
  infoDialog->exec();
}


void FetCamImage::importWebcamList()
{
#ifdef ANDROID_OS
  loadFileDialog->reposition();  // Needed for Qt/Android
#endif // ANDROID_OS
  loadFileDialog->exec();
}


void FetCamImage::nextImage()
{
  selectorDialog->chooseNextItem();
}


void FetCamImage::prevImage()
{
  selectorDialog->choosePreviousItem();
}


void FetCamImage::openHomepage()
{
  QDesktopServices::openUrl(QUrl(currentWebcamHomepage));
}


void FetCamImage::loadWebcam(
  FetCamWidgetItem *item)
{
  currentWebcamUrl = item->getLink();
  currentWebcamHomepage = item->getHomepage();
  emit newWebcamName(item->getName());

  retrieveImage();

  timer->start(item->getRefreshRate() * 1000);
}


void FetCamImage::setImage(
  const QByteArray &imageByteArray)
{
  // Clear any existing image data:
  if (currentMovie)
  {
    delete currentMovie;
    currentMovie = 0;
  }

  if (imageBuffer)
  {
    delete imageBuffer;
    imageBuffer = 0;
  }

  if (imageData)
  {
     delete imageData;
  }

  imageData = new QByteArray(imageByteArray);

  imageBuffer = new QBuffer(imageData);

  QImageReader ir(imageBuffer);

  // Sanity check:
  if (!ir.canRead())
  {
    emit imageError("Unable to display webcam picture");
    return;
  }

  // Check whether the image is animated:
  if (ir.imageCount() > 1)
  {
    // This is an animation:
    currentMovie = new QMovie(imageBuffer);

    QSize size(ir.size());
    size.scale(width(), height(), Qt::KeepAspectRatio);
    currentMovie->setScaledSize(size);

    setMovie(currentMovie);
    currentMovie->start();
  }
  else
  {
    // Display a static picture:
    currentPixmap.loadFromData(imageByteArray);

    setPixmap(
      currentPixmap.scaled(width(), height(), Qt::KeepAspectRatio));
  }
}


void FetCamImage::manualResize()
{
  QWidget *p = parentWidget();

  // Sanity check:
  if (!p) return;

  resize(p->size());
}


void FetCamImage::resizeEvent(
  QResizeEvent *event)
{

  // Another sanity check:
  if (!imageData) return;

  // Repaint the image using the current geometry:
  if (currentMovie)
  {
    // Recreate the movie:
    if (imageBuffer) delete imageBuffer;
    imageBuffer = new QBuffer(imageData);

    delete currentMovie;
    currentMovie = new QMovie(imageBuffer);

    // Resize the movie:
    QImageReader ir(imageBuffer);
    QSize size(ir.size());
    size.scale(width(), height(), Qt::KeepAspectRatio);
    currentMovie->setScaledSize(size);

    // Run the movie:
    setMovie(currentMovie);
    currentMovie->start();
  }
  else
  {
    setPixmap(
      currentPixmap.scaled(width(), height(), Qt::KeepAspectRatio));
  }

  event->accept();
}
