#include "fetloadfiledialog.h"
#include "ui_fetloadfiledialog.h"

#include "fettargetarchitecture.h"
#include <QXmlStreamReader>
#include <QNetworkReply>
#include <QProgressDialog>
#include <QFileDialog>
#include <QFile>

#include <QDebug>

FetLoadFileDialog::FetLoadFileDialog(
  QWidget *parent)
  : QDialog(parent),
    ui(new Ui::FetLoadFileDialog),
    xmlFileReply(0),
    downloadQPD(0),
    canceled(false)
{
  ui->setupUi(this);

  downloadQPD = new QProgressDialog();

  // For now, we are downloading a fixed web page.  This will change!!
  downloadQPD->setLabelText("Downloading fettuccine.xml");

  connect(
    downloadQPD,
    SIGNAL(canceled()),
    this,
    SLOT(cancelCurrentDownload()));
}


FetLoadFileDialog::~FetLoadFileDialog()
{
  delete ui;
}


void FetLoadFileDialog::on_buttonBox_accepted()
{
  // Need to do better than accessing raw index values here!
  if (ui->sourceComboBox->currentIndex() == 0)
  {
    // Load file from the web.  Need to make this editable by user!!
    xmlFileReply = xmlFileRetriever.get(
      QNetworkRequest(
        QUrl(
          "http://jpietrzak8.github.io/Fettuccine/fettuccine.xml")));

    connect(
      xmlFileReply,
      SIGNAL(finished()),
      this,
      SLOT(marshalReply()));

    connect(
      xmlFileReply,
      SIGNAL(downloadProgress(qint64, qint64)),
      this,
      SLOT(downloadProgress(qint64, qint64)));

    downloadQPD->exec();

    // For debugging purposes:
    filename = "fettuccine.xml";
    
    return;
  }

  // Load local file.

  filename = QFileDialog::getOpenFileName(
    this, "Select Webcam XML File");

  if (filename.isEmpty())
  {
    // The user did not choose a file.
    return;
  }

  QFile webcamFile(filename);

  if (!webcamFile.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    QString errString;
    errString += "Unable to open ";
    errString += filename;
    emit loadFileError(errString);
qDebug() << "Unable to open " << filename;
    return;
  }

  readWebcamFile(&webcamFile);
}


void FetLoadFileDialog::downloadProgress(
  qint64 bytesReceived,
  qint64 bytesTotal)
{
  if (canceled) return;

  downloadQPD->setMaximum(bytesTotal);
  downloadQPD->setValue(bytesReceived);
}


void FetLoadFileDialog::cancelCurrentDownload()
{
  canceled = true;

  xmlFileReply->abort();
}


void FetLoadFileDialog::marshalReply()
{
  if (!canceled)
  {
    // Throw error if reply empty?
    if (xmlFileReply)
    {
      readWebcamFile(xmlFileReply);

      xmlFileReply->deleteLater();
    }
  }

  // Reset the canceled flag:
  canceled = false;

  // Hide the progress bar:
  downloadQPD->hide();
}


void FetLoadFileDialog::readWebcamFile(
  QIODevice *webcamFile)
{
  QXmlStreamReader webcamReader(webcamFile);

  FetCamCollection camList;

  parseWebcamXml(webcamReader, camList);

  if (camList.isEmpty())
  {
    QString errString;
    errString += "No webcams found in ";
    errString += filename;
    emit loadFileError(errString);
qDebug() << "No Webcams Found in " << filename;;
    return;
  }

  emit newCamList(camList);
}


void FetLoadFileDialog::parseWebcamXml(
  QXmlStreamReader &webcamReader,
  FetCamCollection &camList)
{
  while(!webcamReader.atEnd())
  {
    webcamReader.readNext();

    if (webcamReader.isStartElement())
    {
      if (webcamReader.name() == "fettuccine")
      {
//qDebug() << "found fettuccine element";
        parseFettuccineElement(webcamReader, camList);
      }
    }
  }

  if (webcamReader.hasError())
  {
    QString err;
    err.append("QXmlStreamReader returned error: ");
    err.append(webcamReader.errorString());
    emit loadFileError(err);
    return;
  }
}


void FetLoadFileDialog::parseFettuccineElement(
  QXmlStreamReader &webcamReader,
  FetCamCollection &camList)
{
  QString link;
  QString homepage;
  int refreshRate = 30; // default to 30 seconds.

  while (!webcamReader.atEnd())
  {
    webcamReader.readNext();

    if (webcamReader.isStartElement())
    {
      if (webcamReader.name() == "webcam")
      {
        if (webcamReader.attributes().hasAttribute("link"))
        {
          link = webcamReader.attributes().value("link").toString();
        }

        if (webcamReader.attributes().hasAttribute("homepage"))
        {
          homepage = webcamReader.attributes().value("homepage").toString();
        }

        if (webcamReader.attributes().hasAttribute("refreshRate"))
        {
          // Qt 4.8 needs conversion to string to get int, Qt 5.0 doesn not.
#ifdef MAEMO_OS
          refreshRate = webcamReader.attributes().value("refreshRate").toString().toInt();
#else
          refreshRate = webcamReader.attributes().value("refreshRate").toInt();
#endif // MAEMO_OS
        }

        FetCamWidgetItem *item =
          new FetCamWidgetItem(link, homepage, refreshRate);

        parseWebcamElement(webcamReader, item);

        camList.append(item);

        link.clear();
        homepage.clear();
        refreshRate = 30;
      }
    }

    else if (webcamReader.isEndElement())
    {
      if (webcamReader.name() == "fettuccine")
      {
        break;
      }
    }
  }
}


void FetLoadFileDialog::parseWebcamElement(
  QXmlStreamReader &webcamReader,
  FetCamWidgetItem *item)
{
  QString textString;

  while (!webcamReader.atEnd())
  {
    webcamReader.readNext();

    if (webcamReader.isStartElement())
    {
      if (webcamReader.name() == "name")
      {
        textString = parseText(webcamReader, "name");
        if (!textString.isEmpty())
        {
          item->setName(textString);
        }
      }
      else if (webcamReader.name() == "tag")
      {
        textString = parseText(webcamReader, "tag");
        if (!textString.isEmpty())
        {
          item->addTag(textString);
        }
      }
    }
    else if (webcamReader.isEndElement())
    {
      if (webcamReader.name() == "webcam")
      {
        break;
      }
    }
  }
}


QString FetLoadFileDialog::parseText(
  QXmlStreamReader &webcamReader,
  QString elementName)
{
  QString textString;

  while (!webcamReader.atEnd())
  {
    webcamReader.readNext();

    if (webcamReader.isCharacters() && !webcamReader.isWhitespace())
    {
      textString.append(webcamReader.text().toString());
    }
    else if (webcamReader.isEndElement())
    {
      if (webcamReader.name() == elementName)
      {
        break;
      }
    }
  }

  return textString;
}
