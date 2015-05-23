//
// fetwidgetuiform.h
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

#ifndef FETWIDGETUIFORM_H
#define FETWIDGETUIFORM_H

#include <QWidget>

namespace Ui {
class FetWidgetUIForm;
}

class FetCamImage;
class FetWidgetSettingsDialog;
class QResizeEvent;

class FetWidgetUIForm : public QWidget
{
  Q_OBJECT
  
public:
  explicit FetWidgetUIForm(QWidget *parent = 0);
  ~FetWidgetUIForm();

  QSize sizeHint() const;

  void setupSettingsDialog();

public slots:
  void showSettingsDialog();

protected:
  void resizeEvent(
    QResizeEvent *event);
  
private slots:
  void on_nextButton_clicked();
  void on_prevButton_clicked();

  void changeSizeTo(
    int width,
    int height);

  void displayNavButtons(
    bool showButtons);

  void displayTitle(
    bool showTitle);

  void setWebcamName(
    QString name);

private:
  Ui::FetWidgetUIForm *ui;

  FetCamImage *webcamImage;
  FetWidgetSettingsDialog *settingsDialog;
};

#endif // FETWIDGETUIFORM_H
