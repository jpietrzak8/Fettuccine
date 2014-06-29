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

class QWidget;
class QByteArray;
class QBuffer;
class QMovie;

class FetCamImage: public QLabel
{
  Q_OBJECT

public:
  FetCamImage(
    QWidget *parent);

  ~FetCamImage();

  void setImage(
    const QByteArray &imageByteArray);

  void manualResize();

signals:
  void imageError(QString);

protected:
  void resizeEvent(
    QResizeEvent *event);

private:
  QByteArray *imageData;
  QBuffer *imageBuffer;
  QMovie *currentMovie;
  QPixmap currentPixmap;
};

#endif // FETCAMIMAGE_H
