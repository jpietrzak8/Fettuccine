#ifndef FETABOUTDIALOG_H
#define FETABOUTDIALOG_H

#include <QDialog>

namespace Ui {
class FetAboutDialog;
}

class FetAboutDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit FetAboutDialog(QWidget *parent = 0);
  ~FetAboutDialog();
  
private:
  Ui::FetAboutDialog *ui;
};

#endif // FETABOUTDIALOG_H
