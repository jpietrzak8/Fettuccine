//
// fetcaminfodialog.h
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

#ifndef FETCAMINFODIALOG_H
#define FETCAMINFODIALOG_H

#include <QDialog>

#include "fettargetarchitecture.h"

class FetCamWidgetItem;
class QResizeEvent;

namespace Ui {
class FetCamInfoDialog;
}

class FetCamInfoDialog : public QDialog
{
  Q_OBJECT

public:
  FetCamInfoDialog(
    QWidget *parent);

  ~FetCamInfoDialog();

  void setupDialog(
    FetCamWidgetItem *item);

#ifdef ANDROID_OS
  void reposition();
#endif // ANDROID_OS

protected:
#ifdef ANDROID_OS
  virtual void resizeEvent(
    QResizeEvent *e);
#endif // ANDROID_OS

private:
  Ui::FetCamInfoDialog *ui;
};

#endif // FETCAMINFODIALOG_H
