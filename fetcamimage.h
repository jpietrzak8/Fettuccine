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

class QWidget;
class QByteArray;
class QBuffer;
class QMovie;
class FetMessenger;
class FetCamSelectorDialog;
class FetTagDialog;
class FetCamInfoDialog;
class FetLoadFileDialog;
class FetWidgetSettingsDialog;
class FetCamWidgetItem;
class QTimer;
class QNetworkReply;
class QResizeEvent;

class FetCamImage: public QLabel
{
  Q_OBJECT

public:
  explicit FetCamImage(
    QWidget *parent = 0);

  ~FetCamImage();

  QSize sizeHint() const;

  void setupSettingsDialog();

  void setImage(
    const QByteArray &imageByteArray);

  void manualResize();

  void nextImage();
  void prevImage();
  void openHomepage();

public slots:
  void selectWebcam();
  void filterByCategory();
  void displayWebcamInfo();
  void importWebcamList();
  void showSettingsDialog();

  void changeSizeTo(
    int width,
    int height);

signals:
  void imageError(QString);

  void newWebcamName(QString);

protected:
  void resizeEvent(
    QResizeEvent *event);

private slots:
  void retrieveImage();

  void updateWidget(
    QNetworkReply *reply);

  void loadWebcam(
    FetCamWidgetItem *item);

private:
  QByteArray *imageData;
  QBuffer *imageBuffer;
  QMovie *currentMovie;
  QPixmap currentPixmap;

  FetCamSelectorDialog *selectorDialog;
  FetTagDialog *tagDialog;
  FetCamInfoDialog *infoDialog;
  FetLoadFileDialog *loadFileDialog;
  FetWidgetSettingsDialog *settingsDialog;
  FetMessenger *messenger;

  QNetworkAccessManager qnam;
  QNetworkReply *currentReply;

  QTimer *timer;

  QString currentWebcamUrl;
  QString currentWebcamHomepage;
  int numberOfRedirections;
};

#endif // FETCAMIMAGE_H
