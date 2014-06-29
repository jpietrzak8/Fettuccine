//
// fettagdialog.cpp
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

#include "fettagdialog.h"
#include "ui_fettagdialog.h"

#include "fettargetarchitecture.h"
#include <QListWidgetItem>
#include <QPushButton>
#include <QDialogButtonBox>

#ifdef ANDROID_OS
#include <QScroller>
#endif // ANDROID_OS

#include <QDebug>

FetTagDialog::FetTagDialog(
  QWidget *parent)
  : QDialog(parent),
    ui(new Ui::FetTagDialog)
{
  ui->setupUi(this);

#ifdef ANDROID_OS
  QScroller::grabGesture(
    ui->webcamListWidget,
    QScroller::LeftMouseButtonGesture);
#endif // ANDROID_OS

  // Add a "no tags" option to button box:
  QPushButton *noTagsButton = new QPushButton("No Filtering");
  ui->buttonBox->addButton(noTagsButton, QDialogButtonBox::AcceptRole);

  connect(
    noTagsButton,
    SIGNAL(clicked()),
    this,
    SLOT(removeFilter()));
}


FetTagDialog::~FetTagDialog()
{
  delete ui;
}


void FetTagDialog::addTag(
  QString tag)
{
  // Only add the tag if it isn't already there:
  if (ui->tagListWidget->findItems(tag, Qt::MatchFixedString).count() > 0)
  {
    // Tag already added.
    return;
  }

  ui->tagListWidget->addItem(tag);
}


void FetTagDialog::removeFilter()
{
  currentTag.clear();

  ui->tagListWidget->setCurrentRow(-1);

  emit selectByTag(currentTag);

  // Is this needed?
//  accept();
}


void FetTagDialog::on_tagListWidget_itemClicked(
  QListWidgetItem *item)
{
  currentTag = item->text();

  emit selectByTag(currentTag);

  accept();
}
