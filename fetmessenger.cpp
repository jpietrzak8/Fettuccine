//
// fetmessenger.cpp
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

#include "fetmessenger.h"

#include "fettargetarchitecture.h"

#include <QWidget>

#ifdef MAEMO_OS
#include <QMaemo5InformationBox>
#endif // MAEMO_OS

#ifdef ANDROID_OS
//#include <QAndroidJniObject>
#include <QMessageBox>
#endif // ANDROID_OS

FetMessenger::FetMessenger(
  QWidget *w)
  : parentWidget(w)
{
}


void FetMessenger::sendMessage(
  QString message)
{
#ifdef MAEMO_OS
  QMaemo5InformationBox::information(parentWidget, message);
#endif // MAEMO_OS

#ifdef ANDROID_OS
  QMessageBox mb;
  mb.setText(message);
  mb.setStandardButtons(QMessageBox::Ok);

  mb.show();

  QPoint p = parentWidget->mapToGlobal(
    QPoint(
      (parentWidget->width() - mb.width())/2,
      (parentWidget->height() - mb.height())/2));

  mb.move(p);

  mb.exec();

#endif // ANDROID_OS
}
