//
// fetcaminfodialog.cpp
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

#include "fetcaminfodialog.h"
#include "ui_fetcaminfodialog.h"

//#include "fettargetarchitecture.h"

#include "fetcamwidgetitem.h"

#include <QDebug>

FetCamInfoDialog::FetCamInfoDialog(
  QWidget *parent)
  : QDialog(parent),
    ui(new Ui::FetCamInfoDialog)
{
  ui->setupUi(this);

//  setWindowState(windowState() | Qt::WindowMaximized);
}


FetCamInfoDialog::~FetCamInfoDialog()
{
  delete ui;
}


void FetCamInfoDialog::setupDialog(
  FetCamWidgetItem *item)
{
  if (!item) return;

  QString name = item->getName();

  QString link;
  link += "<a href=\"";
  link += item->getLink();
  link += "\">";
  link += item->getLink();
  link += "</a>";

  QString homepage;
  homepage += "<a href=\"";
  homepage += item->getHomepage();
  homepage += "\">";
  homepage += item->getHomepage();
  homepage += "</a>";

  if (name.isEmpty())
  {
    ui->nameLabel->setText(link);
  }
  else
  {
    ui->nameLabel->setText(name);
  }

  ui->linkLabel->setText(link);

  if (item->getHomepage().isEmpty())
  {
    ui->homepageLabel->setText(link);
  }
  else
  {
    ui->homepageLabel->setText(homepage);
  }

  ui->rateLabel->setText(QString::number(item->getRefreshRate()));
}

#ifdef ANDROID_OS
void FetCamInfoDialog::resizeEvent(
  QResizeEvent *e)
{
  // First perform the normal resize:
  QDialog::resizeEvent(e);

  // Then, move the dialog to the middle:

  QWidget *parent = parentWidget();

  if (!parent) return;

  move(
    (parent->width() - width()) / 2,
    (parent->height() - height()) / 2);
}
#endif // ANDROID_OS


#ifdef ANDROID_OS
void FetCamInfoDialog::reposition()
{
  QWidget *parent = parentWidget();

  if (!parent) return;

  move(
    (parent->width() - width()) / 2,
    (parent->height() - height()) / 2);
}
#endif // ANDROID_OS
