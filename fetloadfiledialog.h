#ifndef FETLOADFILEDIALOG_H
#define FETLOADFILEDIALOG_H

#include <QDialog>

#include <QString>
#include "fetcamwidgetitem.h"
#include <QNetworkAccessManager>

class QXmlStreamReader;
class QNetworkReply;
class QProgressDialog;

namespace Ui {
class FetLoadFileDialog;
}

class FetLoadFileDialog : public QDialog
{
  Q_OBJECT
  
public:
  explicit FetLoadFileDialog(QWidget *parent = 0);
  ~FetLoadFileDialog();

  void loadDefaultList();

signals:
  void newCamList(
    FetCamCollection camList);

  void loadFileError(
    QString err);
  
private slots:
  void on_buttonBox_accepted();

  void on_sourceComboBox_activated(
    const QString &arg1);

  void downloadProgress(
    qint64 bytesReceived,
    qint64 bytesTotal);

  void cancelCurrentDownload();

  void marshalReply();

private:
  void loadFile();

  void readWebcamFile(
    QIODevice *webcamFile);

  void parseWebcamXml(
    QXmlStreamReader &webcamReader,
    FetCamCollection &camList);

  void parseFettuccineElement(
    QXmlStreamReader &webcamReader,
    FetCamCollection &camList);

  void parseWebcamElement(
    QXmlStreamReader &webcamReader,
    FetCamWidgetItem *item);

  QString parseText(
    QXmlStreamReader &webcamReader,
    QString elementName);

  Ui::FetLoadFileDialog *ui;

  QNetworkAccessManager xmlFileRetriever;
  QNetworkReply *xmlFileReply;
  QProgressDialog *downloadQPD;
  bool canceled;
  QString filename;
};

#endif // FETLOADFILEDIALOG_H
