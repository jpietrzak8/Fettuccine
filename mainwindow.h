//
// mainwindow.h
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "fettargetarchitecture.h"

#ifdef MAEMO_OS
#include <QtGui/QMainWindow>
#else
#include <QMainWindow>
#endif // MAEMO_OS

class QResizeEvent;
class FetCamImage;

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(
    QWidget *parent = 0);

  virtual ~MainWindow();

protected:
  void resizeEvent(
    QResizeEvent *event);

private slots:
  void on_actionSelect_Webcam_triggered();
  void on_actionWebcam_Info_triggered();
  void on_actionImport_Webcam_List_triggered();
  void on_actionFilter_by_Category_triggered();
  void on_actionDocumentation_triggered();
  void on_actionAbout_triggered();
  void on_fFullscreenButton_clicked();
  void on_hFullscreenButton_clicked();
  void on_hNextButton_clicked();
  void on_hPrevButton_clicked();
  void on_vPrevButton_clicked();
  void on_vNextButton_clicked();
  void on_vFullscreenButton_clicked();
//  void on_vHomeButton_clicked();
//  void on_hHomeButton_clicked();
  void on_hTagButton_clicked();
  void on_vTagButton_clicked();
  void on_hPlayPauseButton_clicked();
  void on_vPlayPauseButton_clicked();
  void on_closeAboutButton_clicked();
  void on_closeDocButton_clicked();

  void setWebcamName(
    QString name);

  void pauseViewer();

private:
  void returnToControls();

#ifdef ANDROID_OS
  void resizeIcons();
#endif // ANDROID_OS

  void switchPlayPause();

  Ui::MainWindow *ui;

  bool controlsHidden;
  bool paused;

  // Image management:
  FetCamImage *webcamImage;
};

#endif // MAINWINDOW_H
