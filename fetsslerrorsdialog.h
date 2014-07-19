#ifndef FETSSLERRORSDIALOG_H
#define FETSSLERRORSDIALOG_H

#include <QDialog>
#include <QList>
#include <QSslError>

namespace Ui {
class FetSSLErrorsDialog;
}

class FetSSLErrorsDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit FetSSLErrorsDialog(QWidget *parent = 0);
  ~FetSSLErrorsDialog();

  void setErrors(
    QList<QSslError> errors);

  bool continueLoadingImage() {return continueLoading;}
  
private slots:
  void on_buttonBox_accepted();

  void on_buttonBox_rejected();

private:
  Ui::FetSSLErrorsDialog *ui;

  bool continueLoading;
};

#endif // FETSSLERRORSDIALOG_H
