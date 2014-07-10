//
// fetdbus.cpp
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

#include "fetdbus.h"

#include <QDBusConnection>

//#include <QDebug>


FetDBus::FetDBus()
  : sleeping(false)
{
  QDBusConnection::systemBus().connect(
    "",
    "/com/nokia/mce/signal",
    "com.nokia.mce.signal",
    "display_status_ind",
    this,
    SLOT(displayStatusChanged(QString)));

  QDBusConnection::systemBus().connect(
    "",
    "/com/nokia/mce/signal",
    "com.nokia.mce.signal",
    "tklock_mode_ind",
    this,
    SLOT(displayStatusChanged(QString)));
}


void FetDBus::displayStatusChanged(
  QString status)
{
  if (status == "off" && sleeping == false)
  {
    emit timeToSleep();
    sleeping = true;
    return;
  }
  else if (status == "locked")
  {
    // Should anything be done in this case?
    return;
  }
  else if (sleeping == true)
  {
    emit timeToWakeUp();
    sleeping = false;
  }
}
