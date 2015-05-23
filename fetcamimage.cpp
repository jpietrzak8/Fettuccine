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
#include "fetmessenger.h"
#include <QUrl>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QSettings>
#include "fetauthenticationdialog.h"
#include <QAuthenticator>
#include "fetsslerrorsdialog.h"

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
    messenger(0),
    currentReply(0),
    currentWebcamType(Static_Webcam),
    currentTimerInterval(5000),
    numberOfRedirections(0),
    networkAccessible(true),
    sleeping(false),
    authDialog(0),
    mjpegState(ParsingBoundary_State),
    streamType(Unknown_Stream),
    slideshowTimerInterval(1000),
    currentMJpegImageSize(0),
    maxMJpegListSize(1)
{
  setAlignment(Qt::AlignCenter);

  selectorDialog = new FetCamSelectorDialog(parent);
  tagDialog = new FetTagDialog(parent);
  infoDialog = new FetCamInfoDialog(parent);
  loadFileDialog = new FetLoadFileDialog(parent);
  messenger = new FetMessenger(parent);
  authDialog = new FetAuthenticationDialog(parent);

  // Handle Selector Dialog signals:
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

  // Handle Filter Dialog Signals:
  connect(
    tagDialog,
    SIGNAL(selectByTag(QString)),
    selectorDialog,
    SLOT(filterList(QString)));

  // Handle Load File Dialog signals:
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

  // Handle QNetworkAccessManager signals:
  connect(
    &qnam,
    SIGNAL(finished(QNetworkReply *)),
    this,
    SLOT(updateWidget(QNetworkReply *)));

  connect(
    &qnam,
    SIGNAL(
      networkAccessibleChanged(
        QNetworkAccessManager::NetworkAccessibility)),
    this,
    SLOT(
      parseNetworkAccessibility(
        QNetworkAccessManager::NetworkAccessibility)));

  connect(
    &qnam,
    SIGNAL(
      authenticationRequired(
        QNetworkReply*, QAuthenticator*)),
    this,
    SLOT(
      performAuthentication(
        QNetworkReply*, QAuthenticator*)));

  // Manage Timer signals:
  connect(
    &timer,
    SIGNAL(timeout()),
    this,
    SLOT(retrieveImage()));

  connect(
    &slideshowTimer,
    SIGNAL(timeout()),
    this,
    SLOT(displayNextSlide()));

  if (!selectorDialog->populateList())
  {
    loadFileDialog->loadDefaultList();
  }

#ifdef MAEMO_OS
  // Turn off Fettuccine when the display goes dark:
  connect(
    &dbusListener,
    SIGNAL(timeToSleep()),
    this,
    SLOT(enterSleepMode()));

  // Turn back on when the screen comes back up:
  connect(
    &dbusListener,
    SIGNAL(timeToWakeUp()),
    this,
    SLOT(exitSleepMode()));
#endif // MAEMO_OS
}


FetCamImage::~FetCamImage()
{
  QSettings settings("pietrzak.org", "Fettuccine");
 
  settings.setValue("currentWebcamID", selectorDialog->getCurrentID());

  if (messenger) delete messenger;
  if (loadFileDialog) delete loadFileDialog;
  if (infoDialog) delete infoDialog;
  if (tagDialog) delete tagDialog;
  if (selectorDialog) delete selectorDialog;
  if (currentMovie) delete currentMovie;
  if (imageBuffer) delete imageBuffer;
  if (imageData) delete imageData;
}


void FetCamImage::loadFirstImage()
{
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


QSize FetCamImage::sizeHint() const
{
  return QSize(320, 200);
}


void FetCamImage::resizeDialogs(
  int dialogWidth,
  int dialogHeight)
{
  selectorDialog->resize(dialogWidth, dialogHeight);
  tagDialog->resize(dialogWidth, dialogHeight);
  infoDialog->resize(dialogWidth, dialogHeight);
}


void FetCamImage::sendMessage(
  QString message)
{
  messenger->sendMessage(message);
}


void FetCamImage::retrieveImage()
{
  if (currentReply)
  {
    currentReply->abort();
    currentReply->deleteLater();
  }

  // Stop any ongoing slideshow:
  slideshowTimer.stop();

//qDebug() << "currentWebcamUrl: " << currentWebcamUrl;
  currentReply = qnam.get(QNetworkRequest(QUrl(currentWebcamUrl)));

  connect(
    currentReply,
    SIGNAL(error(QNetworkReply::NetworkError)),
    this,
    SLOT(handleReplyError(QNetworkReply::NetworkError)));

  connect(
    currentReply,
    SIGNAL(sslErrors(QList<QSslError>)),
    this,
    SLOT(handleReplySslErrors(QList<QSslError>)));

  if (currentWebcamType == MJpeg_Webcam)
  {
    // Clear any existing MJpeg data:
    mjpegPixmaps.clear();

    // Reset the parser state to search for the first boundary:
    mjpegState = ParsingBoundary_State;
    streamType = Unknown_Stream;

    // parse data as it comes in, rather than waiting for the end:
    connect(
      currentReply,
      SIGNAL(readyRead()),
      this,
      SLOT(parseMJpegData()));
  }
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
  switch (currentWebcamType)
  {
  case MJpeg_Webcam:
    startupSlideshow();
    break;

  case Static_Webcam:
  default:
    setImage(reply->readAll());
    break;
  }

  reply->deleteLater();
  currentReply = 0; // this is a hack
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
  currentWebcamType = item->getWebcamType();
  maxMJpegListSize = item->getMaxImages();
  slideshowTimerInterval = item->getSlideshowDelay() * 1000;
  emit newWebcamName(item->getName());
  currentTimerInterval = item->getRefreshRate() * 1000;

  retrieveImage();

  timer.start(currentTimerInterval);
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
    messenger->sendMessage("Unable to display webcam picture");
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
  if (currentWebcamType == Static_Webcam)
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
  }
  else
  {
    displaySlide();
  }

  event->accept();
}


void FetCamImage::parseNetworkAccessibility(
  QNetworkAccessManager::NetworkAccessibility access)
{
  if (access == QNetworkAccessManager::Accessible)
  {
    // If the network was previously inaccessible, restart:
    if (!networkAccessible)
    {
      networkAccessible = true;

      // But only restart if we are not sleeping:
      if (!sleeping)
      {
        timer.start(currentTimerInterval);
      }
    }
  }
  else
  {
    // If the network was previously accessible, stop:
    if (networkAccessible)
    {
      networkAccessible = false;

      timer.stop();

      if (currentReply)
      {
        currentReply->abort();
        currentReply->deleteLater();
        currentReply = 0;
      }
    }
  }
}


void FetCamImage::enterSleepMode()
{
  // Shut down the timer, and abort any current requests:
  sleeping = true;

  timer.stop();

  if (currentReply)
  {
    currentReply->abort();
    currentReply->deleteLater();
    currentReply = 0;
  }

  slideshowTimer.stop();
}


void FetCamImage::exitSleepMode()
{
  sleeping = false;

  // First check if the network is accessible:
  if (networkAccessible)
  {
    // Refresh the image, and start the timer back up again.
    retrieveImage();
    timer.start(currentTimerInterval);
  }
  // The following is a hack to get around a QNAM problem:
  else if (qnam.networkAccessible() ==
    QNetworkAccessManager::UnknownAccessibility)
  {
    // Force the QNAM to wake up:
    retrieveImage();
  }
}


void FetCamImage::performAuthentication(
  QNetworkReply *reply,
  QAuthenticator *auth)
{
//qDebug() << reply->readAll();
  authDialog->exec();

  if (authDialog->wasAccepted())
  {
    auth->setUser(authDialog->getUser());
    auth->setPassword(authDialog->getPassword());
  }
  else
  {
    // Cancel the current request:
    timer.stop();
    if (currentReply)
    {
      currentReply->abort();
      currentReply->deleteLater();
      currentReply = 0;
    }
  }
}


void FetCamImage::handleReplyError(
  QNetworkReply::NetworkError error)
{
  switch (error)
  {
  case 5:
    // This is a notification that the reply was aborted/closed.  Since
    // I'll be doing that regularly here, this is not a problem.
    break;

  default:
    qWarning() << "QNetworkReply Error: " << error;
    break;
  }
}


void FetCamImage::handleReplySslErrors(
  QList<QSslError> errors)
{
  FetSSLErrorsDialog sed;
  sed.setErrors(errors);
  sed.exec();

  if (sed.continueLoadingImage())
  {
    currentReply->ignoreSslErrors();
  }
  else
  {
    // Cancel the current request:
    timer.stop();
    if (currentReply)
    {
      currentReply->abort();
      currentReply->deleteLater();
      currentReply = 0;
    }
  }
}


void FetCamImage::parseMJpegData()
{

  if (mjpegState == FinishedParsing_State)
  {
    // Nothing more to do.
    return;
  }

  if (mjpegState == ParsingBoundary_State)
  {
    // If we haven't determined the stream type yet, try to do so:
    if (streamType == Unknown_Stream)
    {
      if (!parseUnknownStreamBoundary()) return;
    }

    // Hopefully, we now know what the boundary type is:
    if (mjpegState == ParsingBoundary_State)
    {
      if (streamType == WebcamXP_Stream)
      {
        if (!parseWebcamXPStreamBoundary()) return;
      }
      else if (streamType == MJpeg_Stream)
      {
        if (!parseGenericStreamBoundary()) return;
      }
    }
  }

  // If we've finished parsing the boundary, start collecting the image:
  if (mjpegState == ParsingJpeg_State)
  {
    parseMJpegImage();
  }
}


bool FetCamImage::parseUnknownStreamBoundary()
{
  if (currentReply->bytesAvailable() < 50)
  {
    // Wait until at least 50 bytes have been read.
    return false;
  }

  QString borderString = currentReply->readLine(51);
//qDebug() << "1. borderString " << borderString;

  if (borderString.startsWith("mjpeg"))
  {
    streamType = WebcamXP_Stream;
    bool flag;
    currentMJpegImageSize = borderString.mid(5,8).toInt(&flag);
    if (!flag)
    {
      qWarning() << "Could not convert " << borderString.mid(5,7) << " to number.";
    }
    else
    {
      mjpegState = ParsingJpeg_State;
    }
  }
  else
  {
    // Other type of stream:
    streamType = MJpeg_Stream;
  }

  return true;
}


bool FetCamImage::parseWebcamXPStreamBoundary()
{
  if (currentReply->bytesAvailable() < 50)
  {
    return false;
  }

  QByteArray borderArray = currentReply->read(51);
  if (!borderArray.startsWith("mjpeg"))
  {
    qWarning() << "invalid border: " << borderArray;
    return false;
  }
  else
  {
    bool flag;
    currentMJpegImageSize = borderArray.mid(5,8).toInt(&flag);
    if (!flag)
    {
      qWarning() << "Could not convert " << borderArray.mid(5,7) << " to number.";
    }
    else
    {
      mjpegState = ParsingJpeg_State;
    }
  }

  return true;
}


bool FetCamImage::parseGenericStreamBoundary()
{
  QString borderString;

  while (currentReply->canReadLine())
  {
    borderString = currentReply->readLine();
//qDebug() << "3. borderString " << borderString;

    if (borderString.startsWith("Content-Length:"))
    {
      bool flag;
      currentMJpegImageSize = borderString.mid(16).toInt(&flag);
      if (!flag)
      {
        qWarning() << "Cound not convert " << borderString.mid(16) << " to number.";
        return false;
      }

      // Try this out for now:
      mjpegState = ParsingJpeg_State;
      // skip the next line:
      borderString = currentReply->readLine();
      break;
    }
/*
    else if (borderString.startsWith("X-Timestamp:"))
    {
      // Try to parse the timestamp:

//      if (timestampRegexp.indexIn(borderString) > -1)
//      {
//        timestampInMs =
//          timestampRegexp.cap(1).toLong() * 1000 +
//          timestampRegexp.cap(2).toLong();
//      }

      mjpegState = ParsingJpeg_State;
      // skip the next line:
      borderString = currentReply->readLine();
      // break out of loop:
      break;
    }
*/
  }

  return true;
}


void FetCamImage::parseMJpegImage()
{
  mjpegByteArray.append(
    currentReply->read(
      currentMJpegImageSize - mjpegByteArray.size()));

//qDebug() << "array size: " << mjpegByteArray.size();

  if (mjpegByteArray.size() == currentMJpegImageSize)
  {
    QPixmap pixmap;
    if (!pixmap.loadFromData(mjpegByteArray))
    {
      qWarning() << "Failed to load MJpeg pixmap";
    }
    else
    {
      // append this image to the list:
      mjpegPixmaps.append(pixmap);
    }
//qDebug() << "got an image";

    // We're now done with this image, reset array 
    mjpegByteArray.clear();

//qDebug() << "mjpegPixmaps.size(): " << mjpegPixmaps.size();
//qDebug() << "maxMJpegListSize: " << maxMJpegListSize;
    // Select next action:
    if (mjpegPixmaps.size() < maxMJpegListSize)
    {
      // look for the next boundary:
      mjpegState = ParsingBoundary_State;
    }
    else
    {
//qDebug() << "Stopping retrieval of images";
      // Don't need any more images:
      mjpegState = FinishedParsing_State;

      // Go ahead and abort the network transaction here,
      // and start the slideshow with what we've got.
      if (currentReply)
      {
        currentReply->abort();
        startupSlideshow();
      }
    }
  }
}


void FetCamImage::startupSlideshow()
{
  // Sanity check:
  if (mjpegPixmaps.empty()) return;

  currentMJpegPixmap = mjpegPixmaps.begin();

  if (maxMJpegListSize > 1)
  {
    slideshowTimer.start(slideshowTimerInterval);
  }

  // Show the first slide immediately:
  currentMJpegPixmap = mjpegPixmaps.begin();
  displaySlide();
}


void FetCamImage::displaySlide()
{
  // Sanity checks:
  if (mjpegPixmaps.empty() || (currentMJpegPixmap == mjpegPixmaps.end()))
  {
    // Nothing to display
    return;
  }

  currentPixmap = *currentMJpegPixmap;

  setPixmap(
    currentPixmap.scaled(width(), height(), Qt::KeepAspectRatio));
}


void FetCamImage::displayNextSlide()
{
  ++currentMJpegPixmap;

  if (currentMJpegPixmap == mjpegPixmaps.end())
  {
    currentMJpegPixmap = mjpegPixmaps.begin();
  }

  displaySlide();
}
