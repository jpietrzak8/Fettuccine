//
// fetcamselectordialog.cpp
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

#include "fetcamselectordialog.h"
#include "ui_fetcamselectordialog.h"

#include <QListWidgetItem>
#include <QRect>
#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>

#ifdef ANDROID_OS
#include <QScroller>
#endif // ANDROID_OS

#include <QDebug>

FetCamSelectorDialog::FetCamSelectorDialog(
  QWidget *parent)
  : QDialog(parent),
    ui(new Ui::FetCamSelectorDialog)
{
  ui->setupUi(this);

//  populateList();

//  setWindowState(windowState() | Qt::WindowMaximized);

#ifdef ANDROID_OS
  QScroller::grabGesture(
    ui->webcamListWidget,
    QScroller::LeftMouseButtonGesture);
#endif // ANDROID_OS
}


FetCamSelectorDialog::~FetCamSelectorDialog()
{
  delete ui;
}


bool FetCamSelectorDialog::populateList()
{
  QSettings settings("pietrzak.org", "Fettuccine");

  int size = settings.beginReadArray("webcams");

  if (size == 0)
  {
    // Initialize the array with some default values:
    settings.endArray();
//    populateDefaultWebcams();
    return false;
  }

  int index = 0;

  QString name;
  QString link;
  QString homepage;
  int refreshRate;
  while (index < size)
  {
    settings.setArrayIndex(index);

    link = settings.value("link").toString();

    name = settings.value("name").toString();
    if (name.isEmpty())
    {
      name = link;
    }

    homepage = settings.value("homepage").toString();
    if (homepage.isEmpty())
    {
      homepage = link;
    }

    refreshRate = settings.value("refresh_rate").toInt();
    if (!refreshRate)
    {
      refreshRate = 30;
    }

    FetCamWidgetItem *item = 
      new FetCamWidgetItem(name, link, homepage, refreshRate);

    // Read the tag array:
    int tagSize = settings.beginReadArray("tags");

    if (tagSize)
    {
      int tagIndex = 0;
      QString tag;
      while (tagIndex < tagSize)
      {
        settings.setArrayIndex(tagIndex);
        tag = settings.value("tag").toString();
        if (!tag.isEmpty())
        {
          item->addTag(tag);
          emit addNewTag(tag);  // Inefficient?
        }

        ++tagIndex;
      }
    }

    settings.endArray();
    // End of the tag array.

    ui->webcamListWidget->addItem(item);

    ++index;
  }

  settings.endArray();

  return true;
}


void FetCamSelectorDialog::populateDefaultWebcams()
{
  // Populate both the QSettings and the list widget at the same time:
  FetCamWidgetItem *item;

  QSettings settings("pietrzak.org", "Fettuccine");

  settings.beginWriteArray("webcams");

  settings.setArrayIndex(0);
  settings.setValue("name", "Old Faithful, Yellowstone");
  settings.setValue("link",
    "http://www.nps.gov/webcams-yell/oldfaithvc.jpg");
  settings.setValue("homepage",
    "http://www.nps.gov/yell/photosmultimedia/webcams.htm");
  settings.setValue("refresh_rate", 30);
  ui->webcamListWidget->addItem(
    new FetCamWidgetItem(
      "Old Faithful",
      "http://www.nps.gov/webcams-yell/oldfaithvc.jpg",
      "http://www.nps.gov/yell/photosmultimedia/webcams.htm",
      30));

  settings.setArrayIndex(1);
  settings.setValue("name", "I-75 at US-35, Ohio");
  settings.setValue("link",
    "http://cmhimg01.dot.state.oh.us/images/dayton/I-75_at_US-35.jpg");
  settings.setValue("homepage",
    "http://www.dot.state.oh.us/districts/D07/TrafficCameras/Pages/I75US35.aspx");
  settings.setValue("refresh_rate", 5);
  settings.beginWriteArray("tags");
  settings.setArrayIndex(0);
  settings.setValue("tag", "Traffic");
  settings.endArray();
  item = new FetCamWidgetItem(
      "I-75 at US-35",
      "http://cmhimg01.dot.state.oh.us/images/dayton/I-75_at_US-35.jpg",
      "http://www.dot.state.oh.us/districts/D07/TrafficCameras/Pages/I75US35.aspx",
      5);
  item->addTag("Traffic");
  ui->webcamListWidget->addItem(item);

  settings.setArrayIndex(2);
  settings.setValue("name", "Kilauea Crater Thermal Image");
  settings.setValue("link",
    "http://hvo.wr.usgs.gov/cams/HTcam/images/M.jpg");
  settings.setValue("homepage",
    "http://hvo.wr.usgs.gov/cams/panorama.php?cam=HTcam");
  settings.setValue("refresh_rate", 30);
  settings.beginWriteArray("tags");
  settings.setArrayIndex(0);
  settings.setValue("tag", "Volcanoes");
  settings.endArray();
  item = new FetCamWidgetItem(
      "Kilauea Crater Thermal Image",
      "http://hvo.wr.usgs.gov/cams/HTcam/images/M.jpg",
      "http://hvo.wr.usgs.gov/cams/panorama.php?cam=HTcam",
      30);
  item->addTag("Volcanoes");
  ui->webcamListWidget->addItem(item);

  settings.setArrayIndex(3);
  settings.setValue("name", "Mt. Susitna, Anchorage, Alaska");
  settings.setValue("link",
    "http://www.borealisbroadband.net/hilton/hilton1.jpg");
  settings.setValue("homepage",
    "http://borealisbroadband.net/vid-hilton1.htm");
  settings.setValue("refresh_rate", 60);
  settings.beginWriteArray("tags");
  settings.setArrayIndex(0);
  settings.setValue("tag", "Mountains");
  settings.endArray();
  ui->webcamListWidget->addItem(
    new FetCamWidgetItem(
      "Mt. Susitna, Anchorage, Alaska",
      "http://www.borealisbroadband.net/hilton/hilton1.jpg",
      "http://borealisbroadband.net/vid-hilton1.htm",
      60));
  item->addTag("Mountains");

  settings.setArrayIndex(4);
  settings.setValue("name", "Tangier Port, Spain");
  settings.setValue("link",
    "http://services.frs-gruppe.de/webcams/images/tangier.jpg");
  settings.setValue("homepage",
    "http://www.frs.de/nc/en/frs-webcams/tangier-morocco.html");
  settings.setValue("refresh_rate", 60);
  ui->webcamListWidget->addItem(
    new FetCamWidgetItem(
      "Tangier Port, Spain",
      "http://services.frs-gruppe.de/webcams/images/tangier.jpg",
      "http://www.frs.de/nc/en/frs-webcams/tangier-morocco.html",
      60));

  settings.setArrayIndex(5);
  settings.setValue("name", "Bosphorus Bridge, Istanbul, Turkey");
  settings.setValue("link",
    "http://icons.wunderground.com/webcamramdisk/z/a/zafer/1/current.jpg");
  settings.setValue("homepage", "");
  settings.setValue("refresh_rate", 30);
  ui->webcamListWidget->addItem(
    new FetCamWidgetItem(
      "Bosphorus Bridge, Istanbul, Turkey",
      "http://icons.wunderground.com/webcamramdisk/z/a/zafer/1/current.jpg",
      "http://icons.wunderground.com/webcamramdisk/z/a/zafer/1/current.jpg",
      30));

  settings.setArrayIndex(6);
  settings.setValue("name", "US Weather Radar");
  settings.setValue("link",
    "http://images.intellicast.com/WxImages/RadarLoop/usa_None_anim.gif");
  settings.setValue("homepage",
    "http://www.intellicast.com/National/Radar/Current.aspx");
  settings.setValue("refresh_rate", 900);
  ui->webcamListWidget->addItem(
    new FetCamWidgetItem(
      "US Weather Radar",
      "http://images.intellicast.com/WxImages/RadarLoop/usa_None_anim.gif",
      "http://www.intellicast.com/National/Radar/Current.aspx",
      900));

  settings.setArrayIndex(7);
  settings.setValue("name", "Cheung Chau, Hong Kong Observatory");
  settings.setValue("link",
    "http://www.hko.gov.hk/wxinfo/aws/hko_mica/cch/latest_CCH.jpg");
  settings.setValue("homepage",
    "http://www.hko.gov.hk/wxinfo/ts/webcam/CCH_e_realtime.htm");
  settings.setValue("refresh_rate", 300);
  ui->webcamListWidget->addItem(
    new FetCamWidgetItem(
      "Cheung Chau, Hong Kong Observatory",
      "http://www.hko.gov.hk/wxinfo/aws/hko_mica/cch/latest_CCH.jpg",
      "http://www.hko.gov.hk/wxinfo/ts/webcam/CCH_e_realtime.htm",
      300));

  settings.endArray();
  settings.sync();
}


/*
void FetCamSelectorDialog::on_webcamListWidget_itemActivated(
  QListWidgetItem *item)
{
  FetCamWidgetItem *cwi = dynamic_cast<FetCamWidgetItem *>(item);

  emit showWebcam(cwi);

  accept();
}
*/


void FetCamSelectorDialog::on_webcamListWidget_itemClicked(
  QListWidgetItem *item)
{
  FetCamWidgetItem *cwi = dynamic_cast<FetCamWidgetItem *>(item);

  emit showWebcam(cwi);

  accept();
}


void FetCamSelectorDialog::chooseItem(
  int index)
{
  // Sanity checks:
  if (index < 0)
  {
    index = 0;
  }

  if (index >= ui->webcamListWidget->count())
  {
    index = ui->webcamListWidget->count()-1;
  }

  ui->webcamListWidget->setCurrentRow(index);

  FetCamWidgetItem *cwi = dynamic_cast<FetCamWidgetItem *>(
    ui->webcamListWidget->item(index));

  emit showWebcam(cwi);
}


void FetCamSelectorDialog::chooseNextItem()
{
  int bound = ui->webcamListWidget->currentRow();
  int index = bound + 1;
  QListWidgetItem *qlwi = 0;

  while (index != bound)
  {
    if (index >= ui->webcamListWidget->count())
    {
      index = 0;
    }

    qlwi = ui->webcamListWidget->item(index);

    if (!qlwi->isHidden())
    {
      // Found a matching webcam:
      break;
    }

    ++index;
  }

  if ((index == bound) || (!qlwi))
  {
    // Can't find any other visible webcams.
    emit selectorWarning("No other webcams found.");
    return;
  }

  ui->webcamListWidget->setCurrentRow(index);

  FetCamWidgetItem *cwi = dynamic_cast<FetCamWidgetItem *>(qlwi);

  emit showWebcam(cwi);
}


void FetCamSelectorDialog::choosePreviousItem()
{
  int bound = ui->webcamListWidget->currentRow();
  int index = bound - 1;
  QListWidgetItem *qlwi = 0;

  while (index != bound)
  {
    if (index < 0)
    {
      index = ui->webcamListWidget->count() - 1;
    }

    qlwi = ui->webcamListWidget->item(index);

    if (!qlwi->isHidden())
    {
      // Found a matching webcam:
      break;
    }

    --index;
  }

  if ((index == bound) || (!qlwi))
  {
    // Can't find any other visible webcams.
    emit selectorWarning("No other webcams found.");
    return;
  }

  ui->webcamListWidget->setCurrentRow(index);

  FetCamWidgetItem *cwi = dynamic_cast<FetCamWidgetItem *>(qlwi);

  emit showWebcam(cwi);
}


int FetCamSelectorDialog::getCurrentID()
{
  return ui->webcamListWidget->currentRow();
}


FetCamWidgetItem *FetCamSelectorDialog::getCurrentItem()
{
  return dynamic_cast<FetCamWidgetItem *>(
    ui->webcamListWidget->currentItem());
}


void FetCamSelectorDialog::loadWebcams(
  FetCamCollection camList)
{
  int size = camList.size();

  // Sanity check:
  if (size == 0) return;

  // Empty out the current list:
  clearWebcams();
  emit clearTags();

  int i = 0;
  while (i < size)
  {
    insertWebcam(camList[i]);
    ++i;
  }

  emit showWebcam(camList[0]);
}


void FetCamSelectorDialog::insertWebcam(
  FetCamWidgetItem *item)
{
  ui->webcamListWidget->addItem(item);

  // set up the item's tags:
  TagCollection::const_iterator i = item->tagsBegin();
  TagCollection::const_iterator end = item->tagsEnd();
  while (i != end)
  {
    emit addNewTag(*i);
    ++i;
  }

  QSettings settings("pietrzak.org", "Fettuccine");

  int size = settings.beginReadArray("webcams");

  settings.endArray();

  settings.beginWriteArray("webcams");

  settings.setArrayIndex(size);
  item->storeData(settings);

  settings.endArray();
  settings.sync();
}


void FetCamSelectorDialog::clearWebcams()
{
  ui->webcamListWidget->clear();

  QSettings settings("pietrzak.org", "Fettuccine");
  settings.remove("webcams");
  settings.sync();
}


void FetCamSelectorDialog::addNewWebcam(
  QString name,
  QString link,
  QString homepage,
  int refreshRate)
{
  FetCamWidgetItem *item = 
    new FetCamWidgetItem(name, link, homepage, refreshRate);

  ui->webcamListWidget->addItem(item);

  emit showWebcam(item);
}


void FetCamSelectorDialog::filterList(
  QString tag)
{
  int count = ui->webcamListWidget->count();
  if (!count)
  {
    // Nothing to do here.
    return;
  }

  int index = 0;
  FetCamWidgetItem *item;

  while (index < count)
  {
    item = dynamic_cast<FetCamWidgetItem *>(
      ui->webcamListWidget->item(index));

    // Do we have a match?
    if (tag.isEmpty() || item->hasTag(tag))
    {
      item->setHidden(false);
    }
    else
    {
      item->setHidden(true);
    }

    ++index;
  }

  // If the list has been reset (so there is no current item), go to
  // the first item on the list.
  if (ui->webcamListWidget->currentRow() == -1)
  {
    ui->webcamListWidget->setCurrentRow(0);
  }

  // If the current item is now hidden, choose the next visible one:
  if (ui->webcamListWidget->currentItem()->isHidden())
  {
    chooseNextItem();
  }
}


#ifdef ANDROID_OS
void FetCamSelectorDialog::resizeEvent(
  QResizeEvent *e)
{
  // Do the standard resize first:
  QDialog::resizeEvent(e);

  // Qt seems to be doing a very very bad job of updating the parent widget's
  // width and height when the screen rotates.  So, we'll see what the
  // orientation is, and swap width for height manually...

  QWidget *parent = parentWidget();

  if (!parent) return;

  move(
    (parent->width() - width()) / 2,
    (parent->height() - height()) / 2);
}
#endif // ANDROID_OS


#ifdef ANDROID_OS
void FetCamSelectorDialog::reposition()
{
  QWidget *parent = parentWidget();

  if (!parent) return;

  move(
    (parent->width() - width()) / 2,
    (parent->height() - height()) / 2);
}
#endif // ANDROID_OS
