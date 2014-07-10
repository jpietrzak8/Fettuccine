//
// fetdbus.h
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

#ifndef FETDBUS_H
#define FETDBUS_H

#include <QObject>

class FetDBus: public QObject
{
  Q_OBJECT

public:
  FetDBus();

signals:
  void timeToSleep();
  void timeToWakeUp();

private slots:
  void displayStatusChanged(
    QString status);

private:
  bool sleeping;
};

#endif // FETDBUS_H
