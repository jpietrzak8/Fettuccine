//
// fetcamwidgetitem.cpp
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

#include "fetcamwidgetitem.h"

//#include <QDebug>

FetCamWidgetItem::FetCamWidgetItem(
  QString n,
  QString l,
  FetWebcamType type,
  int maxImages,
  int sd,
  QString h,
  int r)
  : QListWidgetItem(n),
    displayName(n),
    link(l),
    webcamType(type),
    maxMJpegImages(maxImages),
    slideshowDelay(sd),
    homepage(h),
    refreshRate(r)
{
  setSizeHint(QSize(0, 50));
}


FetCamWidgetItem::FetCamWidgetItem(
  QString l,
  FetWebcamType type,
  int maxImages,
  int sd,
  QString h,
  int r)
  : QListWidgetItem("Unnamed Webcam"),
    displayName("Unnamed Webcam"),
    link(l),
    webcamType(type),
    maxMJpegImages(maxImages),
    slideshowDelay(sd),
    homepage(h),
    refreshRate(r)
{
  setSizeHint(QSize(0, 50));
}


FetCamWidgetItem::FetCamWidgetItem(
  QString n,
  QString l,
  QString h,
  int r)
  : QListWidgetItem(n),
    displayName(n),
    link(l),
    webcamType(Static_Webcam),
    maxMJpegImages(1),
    slideshowDelay(1),
    homepage(h),
    refreshRate(r)
{
  setSizeHint(QSize(0, 50));
}


void FetCamWidgetItem::setName(
  QString name)
{
  displayName = name;

  setText(name);
}


void FetCamWidgetItem::addTag(
  QString tag)
{
  tags.insert(tag);
}


bool FetCamWidgetItem::hasTag(
  QString tag) const
{
  return tags.contains(tag);
}


void FetCamWidgetItem::storeData(
  QSettings &settings)
{
  settings.setValue("name", displayName);
  settings.setValue("link", link);
  settings.setValue("webcamType", webcamType);
  settings.setValue("maxImages", maxMJpegImages);
  settings.setValue("slideshowDelay", slideshowDelay);
  settings.setValue("homepage", homepage);
  settings.setValue("refresh_rate", refreshRate);

  // Do the tags:
  settings.beginWriteArray("tags");

  int index = 0;
  TagCollection::const_iterator iter = tags.begin();
  while (iter != tags.end())
  {
    settings.setArrayIndex(index);
    settings.setValue("tag", *iter);
    ++iter;
    ++index;
  }

  settings.endArray();
}
