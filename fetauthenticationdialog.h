#ifndef FETAUTHENTICATIONDIALOG_H
#define FETAUTHENTICATIONDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class FetAuthenticationDialog;
}

class FetAuthenticationDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit FetAuthenticationDialog(QWidget *parent = 0);
  ~FetAuthenticationDialog();

  bool wasAccepted() {return accepted;}

  QString getUser();
  QString getPassword();
  
private slots:
  void on_buttonBox_accepted();
  void on_buttonBox_rejected();
  void on_showPasswordCheckBox_toggled(bool checked);

private:
  Ui::FetAuthenticationDialog *ui;

  bool accepted;
};

#endif // FETAUTHENTICATIONDIALOG_H
