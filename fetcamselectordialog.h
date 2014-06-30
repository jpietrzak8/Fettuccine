//
// fetcamselectordialog.h
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

#ifndef FETCAMSELECTORDIALOG_H
#define FETCAMSELECTORDIALOG_H

#include <QDialog>

#include "fettargetarchitecture.h"
#include "fetcamwidgetitem.h"

class QListWidgetItem;
class FetCamWidgetItem;
class QResizeEvent;

namespace Ui {
class FetCamSelectorDialog;
}

class FetCamSelectorDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit FetCamSelectorDialog(
    QWidget *parent = 0);

  ~FetCamSelectorDialog();

  void populateList();

  void chooseItem(
    int index);

  void chooseNextItem();
  void choosePreviousItem();

  int getCurrentID();

  FetCamWidgetItem *getCurrentItem();

  void insertWebcam(
    FetCamWidgetItem *item);

  void clearWebcams();

#ifdef ANDROID_OS
  void reposition();
#endif // ANDROID_OS

public slots:
  void addNewWebcam(
    QString name,
    QString link,
    QString homepage,
    int refreshRate);

  void loadWebcams(
    FetCamCollection camList);

  void filterList(
    QString tag);

signals:
  void addNewTag(
    QString tag);

  void clearTags();

  void showWebcam(
    FetCamWidgetItem *);

  void selectorWarning(
    QString);

  void listFiltered();

protected:
#ifdef ANDROID_OS
  virtual void resizeEvent(
    QResizeEvent *e);
#endif // ANDROID_OS

private slots:
//  void on_webcamListWidget_itemActivated(
//    QListWidgetItem *item);

  void on_webcamListWidget_itemClicked(
    QListWidgetItem *item);

private:
  void populateDefaultWebcams();

  Ui::FetCamSelectorDialog *ui;
};

#endif // FETCAMSELECTORDIALOG_H
