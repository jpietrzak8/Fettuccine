//
// main.cpp
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

#include "mainwindow.h"

#include "fettargetarchitecture.h"

#ifdef MAEMO_OS
#include <QtGui/QApplication>
#include <QWidget>
#else
#include <QApplication>
#endif // MAEMO_OS

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  MainWindow mainWindow;

#ifdef MAEMO_OS
//    mainWindow.setAttribute(Qt::WA_AutoOrientation, true);
  mainWindow.setAttribute(static_cast<Qt::WidgetAttribute>(130), true);
  mainWindow.showMaximized();
#else
  mainWindow.show();
#endif // MAEMO_OS

  return app.exec();
}
