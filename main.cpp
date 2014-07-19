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
#include "qmaemo5homescreenadaptor.h"
#include "fetwidgetuiform.h"
#else
#include <QApplication>
#endif // MAEMO_OS

#include <QDebug>

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

#ifdef MAEMO_OS
  // On Maemo, check if we run as a widget:
  bool runAsApp = false;
  QStringList argList = app.arguments();
  int i = 1;
  while (i < argList.size())
  {
    if ( (argList.at(i) == "-a")
      || (argList.at(i) == "--runAsApp"))
    {
      runAsApp = true;
      break;
    }

    ++i;
  }

  if (!runAsApp)
  {
    // Run as a widget:
    FetWidgetUIForm widget;

    widget.setupSettingsDialog();

    QMaemo5HomescreenAdaptor *adaptor =
      new QMaemo5HomescreenAdaptor(&widget);

    adaptor->setSettingsAvailable(true);

    QObject::connect(
      adaptor,
      SIGNAL(settingsRequested()),
      &widget,
      SLOT(showSettingsDialog()));

    widget.show();

    return app.exec();
  }
#endif // MAEMO_OS

  // Run as a normal application:
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
