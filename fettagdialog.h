//
// fettagdialog.h
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

#ifndef FETTAGDIALOG_H
#define FETTAGDIALOG_H

#include <QDialog>

#include <QString>

class QWidget;
class QListWidgetItem;

namespace Ui {
class FetTagDialog;
}

class FetTagDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit FetTagDialog(QWidget *parent = 0);

  ~FetTagDialog();

public slots:
  void addTag(
    QString tag);

  void clearTags();

signals:
  void selectByTag(
    QString);

private slots:
  void removeFilter();

  void on_tagListWidget_itemClicked(
    QListWidgetItem *item);
  
private:
  Ui::FetTagDialog *ui;

  QString currentTag;
};

#endif // FETTAGDIALOG_H
