#ifndef FETWIDGETSETTINGSDIALOG_H
#define FETWIDGETSETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class FetWidgetSettingsDialog;
}

class QListWidgetItem;
class FetAboutDialog;

class FetWidgetSettingsDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit FetWidgetSettingsDialog(
    QWidget *parent = 0);

  ~FetWidgetSettingsDialog();

signals:
  void selectWebcam();
  void filterByCategory();
  void displayWebcamInfo();
  void importWebcamList();
  void newSize(int width, int height);
  void enableNavigation(bool nav);
  void enableTitle(bool title);
  
private slots:
  void on_listWidget_itemClicked(
    QListWidgetItem *item);

  void on_buttonBox_accepted();

  void on_navCheckBox_toggled(bool checked);

  void on_labelCheckBox_toggled(bool checked);

private:
  Ui::FetWidgetSettingsDialog *ui;

  FetAboutDialog *aboutDialog;
};

#endif // FETWIDGETSETTINGSDIALOG_H
