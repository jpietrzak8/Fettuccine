//
// fetcamimage.h
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

#ifndef FETCAMIMAGE_H
#define FETCAMIMAGE_H

#include <QLabel>
#include <QPixmap>
#include <QNetworkAccessManager>
#include "fettargetarchitecture.h"
#include <QList>
#include <QSslError>
#include <QNetworkReply>
#include "fetcamwidgetitem.h"
#include <QTimer>

#ifdef MAEMO_OS
#include "fetdbus.h"
#endif // MAEMO_OS

typedef QList<QPixmap> PixmapList;

class QWidget;
class QByteArray;
class QBuffer;
class QMovie;
class FetMessenger;
class FetCamSelectorDialog;
class FetTagDialog;
class FetCamInfoDialog;
class FetLoadFileDialog;
class QNetworkReply;
class QResizeEvent;
class FetAuthenticationDialog;
class QAuthenticator;

class FetCamImage: public QLabel
{
  Q_OBJECT

public:
  explicit FetCamImage(
    QWidget *parent = 0);

  ~FetCamImage();

  void loadFirstImage();

  QSize sizeHint() const;

  void resizeDialogs(
    int dialogWidth,
    int dialogHeight);

  void setImage(
    const QByteArray &imageByteArray);

  void manualResize();

  void nextImage();
  void prevImage();
  void openHomepage();

/*
  void sendMessage(
    QString message);
*/

public slots:
  void selectWebcam();
  void filterByCategory();
  void displayWebcamInfo();
  void importWebcamList();

  void enterSleepMode();
  void exitSleepMode();

signals:
  void newWebcamName(QString);
  void pauseDisplay();

protected:
  void resizeEvent(
    QResizeEvent *event);

private slots:
  void retrieveImage();

  void updateWidget(
    QNetworkReply *reply);

  void loadWebcam(
    FetCamWidgetItem *item);

  void parseNetworkAccessibility(
    QNetworkAccessManager::NetworkAccessibility access);

  void performAuthentication(
    QNetworkReply *reply,
    QAuthenticator *auth);

  void handleReplyError(
    QNetworkReply::NetworkError error);

  void handleReplySslErrors(
    QList<QSslError> errors);

  void parseMJpegData();

  void displayNextSlide();

private:
  bool parseUnknownStreamBoundary();

  bool parseWebcamXPStreamBoundary();

  bool parseGenericStreamBoundary();

  void parseMJpegImage();

  void startupSlideshow();

  void displaySlide();

  QByteArray *imageData;
  QBuffer *imageBuffer;
  QMovie *currentMovie;
  QPixmap currentPixmap;

  FetCamSelectorDialog *selectorDialog;
  FetTagDialog *tagDialog;
  FetCamInfoDialog *infoDialog;
  FetLoadFileDialog *loadFileDialog;
  FetMessenger *messenger;

  QNetworkAccessManager qnam;
  QNetworkReply *currentReply;
  bool retrievalInProgress; // can ignore net issues when not retrieving data.

  QTimer timer;
  bool paused;

  QString currentWebcamUrl;
  QString currentWebcamHomepage;
  FetWebcamType currentWebcamType;
  int currentTimerInterval;
  int numberOfRedirections;
  bool networkAccessible;

  FetAuthenticationDialog *authDialog;

  // MJpeg data:
  enum FetMjpegState
  {
    ParsingBoundary_State,
    ParsingJpeg_State,
    FinishedParsing_State
  };

  FetMjpegState mjpegState;

  enum FetMjpegStreamType
  {
    Unknown_Stream,
    WebcamXP_Stream,
    MJpeg_Stream
  };

  FetMjpegStreamType streamType;

//  QRegExp timestampRegExp;

  QTimer slideshowTimer;
  int slideshowTimerInterval;
  QByteArray mjpegByteArray;
  int currentMJpegImageSize;
  PixmapList mjpegPixmaps;
  PixmapList::iterator currentMJpegPixmap;
  int maxMJpegListSize;

#ifdef MAEMO_OS
  FetDBus dbusListener;
#endif // MAEMO_OS
};

#endif // FETCAMIMAGE_H
