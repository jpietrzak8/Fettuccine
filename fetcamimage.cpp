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

#include <QDebug>

FetCamImage::FetCamImage(
  QWidget *parent)
  : QLabel(parent),
    imageData(0),
    imageBuffer(0),
    currentMovie(0)
{
  setAlignment(Qt::AlignCenter);
}


FetCamImage::~FetCamImage()
{
  if (currentMovie) delete currentMovie;
  if (imageBuffer) delete imageBuffer;
  if (imageData) delete imageData;
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
