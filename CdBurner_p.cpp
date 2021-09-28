// Qt
#include <QTimer>
#include <QDebug>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFutureWatcher>
#include <QFuture>
#include <QtConcurrent>

// Local
#include "ProcessWrapper.h"
#include "CdBurner_p.h"

static const char* cmdToFindDrivers = "/usr/bin/xorrecord --devices";
static const char* cmdToGenIsoImage = "/usr/bin/genisoimage -joliet-long -U -r -o";
static const char* dirToStoreTmpIsoImage = "/tmp";
static const char* cmdToBurnCd = "/usr/bin/xorrecord -v dev=%1 blank=as_needed";
static const char* bounds = "-----------------------------------------------------------------------------";
static const char* devStr  = "-dev";

CdBurner_P::CdBurner_P(const QString& base_dir, QObject* parent)
  : QObject(parent)
  , m_burnerProcess_(new ProcessWrapper("", QStringList(), this))
  , m_findDevicesProcess_(new ProcessWrapper("", QStringList(), this))
  , m_searchDriversTimer_(new QTimer(this))
  , m_currentCmd(Commands::initCmd)
  , m_baseDir(base_dir)
{
  connect(m_findDevicesProcess_, &ProcessWrapper::stdOut, this, &CdBurner_P::readStdOut);
  connect(m_findDevicesProcess_, &ProcessWrapper::stdError, this, &CdBurner_P::readStdOut);
  connect(m_findDevicesProcess_, &ProcessWrapper::exited, this, &CdBurner_P::processFinished);

  connect(m_burnerProcess_, &ProcessWrapper::stdOut, this, &CdBurner_P::readStdOut);
  connect(m_burnerProcess_, &ProcessWrapper::stdError, this, &CdBurner_P::readStdOut);
  connect(m_burnerProcess_, &ProcessWrapper::exited, this, &CdBurner_P::processFinished);
  connect(m_searchDriversTimer_, &QTimer::timeout, this, &CdBurner_P::searchDrivers);
  m_searchDriversTimer_->start(5000);
}

void CdBurner_P::burnCd(const QString& dev, const QStringList& files)
{
  m_currentCmd = Commands::burnCdCmd;
  m_searchDriversTimer_->stop();

  const QString file_name = QDateTime::currentDateTime().toString("yyyy_MM_ddTHH_mm_ss");
  m_currTmpIsoDir = QString(dirToStoreTmpIsoImage) + "/" + file_name;
  m_currIsoFile = m_currTmpIsoDir + ".iso";
  auto watcher = new QFutureWatcher<void>;
  connect(watcher, &QFutureWatcher<void>::finished, [this, dev, watcher]()
  {
    const QString full_command_to_gen_iso = QString(cmdToGenIsoImage) + " " + m_currIsoFile + " " + m_currTmpIsoDir;
    const QString full_burn_cd_command = QString(cmdToBurnCd).replace("%1", dev) + " " + m_currIsoFile;
    const QString burn_cd_cmd = QString("/bin/bash -c") + " " + "\"" + full_command_to_gen_iso + " ; " + full_burn_cd_command + "\"";
    qInfo() << "command to burn cd" << burn_cd_cmd;
    m_burnerProcess_->start(burn_cd_cmd);
    watcher->deleteLater();
  });
  QFuture<void> copy_files = QtConcurrent::run(this, &CdBurner_P::createTmpRootDir, files, m_currTmpIsoDir, m_baseDir);
  watcher->setFuture(copy_files);
}

void CdBurner_P::setRootDir(const QString& rootDir)
{
  m_baseDir = rootDir;
}

void CdBurner_P::readStdOut(const QString& str)
{
  if (m_currentCmd == Commands::findDriversCmd)
  {
    QStringList devices_str_list = str.split("\n");
    for (auto i = 0; i < devices_str_list.size(); ++i)
    {
      auto devices_str = devices_str_list[i];

      if (devices_str == bounds || devices_str == "")
        continue;

      qDebug() << "new device" << devices_str;
      if (devices_str.contains(devStr))
      {
        QStringList dev_parts = devices_str.split(":");

        if (dev_parts.size() < 2) {
          qWarning() << "cannot parse string and find cdrw/dvdrw:" << devices_str;
          return;
        }

        QRegExp dev_regexp("/dev/[a-z]{2,}[0-9]{0,}");
        auto matched_start = dev_regexp.indexIn(dev_parts[0]);
        auto matched_end = dev_regexp.matchedLength();
        QString device = dev_parts[0].mid(matched_start, matched_end);
        QStringList device_names_list = dev_parts[1].split("'");
        QString dev_name = device_names_list[1].trimmed();
        m_drivers[device] = dev_name;
      }
    }
  }
  else if (m_currentCmd == Commands::burnCdCmd)
  {
    QStringList burn_cd_out_list = str.split("\n");
    for (auto i = 0; i < burn_cd_out_list.size(); ++i) {
      auto burn_cd_str = burn_cd_out_list[i];

      if (burn_cd_str == "")
        continue;
      qDebug() << "new burn str" << burn_cd_str;

      QRegExp progress_regex("(\\d{1,})\\s{1,}of\\s{1,}(\\d{1,})\\s{1,}MB");
      auto matched_start = progress_regex.indexIn(burn_cd_str);
      if (matched_start > -1) {
        QString curr_size_mb = progress_regex.cap(1);
        QString size_of_file_mb = progress_regex.cap(2);
        double progress = 100.0 * curr_size_mb.toDouble() / size_of_file_mb.toDouble();
        emit burnProgress(int(progress));
      }
    }
  }
}

void CdBurner_P::searchDrivers()
{
  if (m_findDevicesProcess_->running())
    return;
  m_currentCmd = Commands::findDriversCmd;
  m_findDevicesProcess_->start(cmdToFindDrivers);
  m_searchDriversTimer_->stop();
}

void CdBurner_P::processFinished()
{
  auto proc = static_cast<ProcessWrapper*>(sender());

  if (m_currentCmd == Commands::findDriversCmd)
  {
    static QHash<QString, QString> drivers;
    m_searchDriversTimer_->start();
    if (drivers != m_drivers) {
      drivers = m_drivers;
    }
    else
    {
      m_drivers.clear();
      return;
    }
    emit listDrivers(drivers);
    m_currentCmd = Commands::initCmd;
  }
  else if (m_currentCmd == Commands::burnCdCmd && proc == m_burnerProcess_)
  {
    QtConcurrent::run(this, &CdBurner_P::cleanup, m_currTmpIsoDir, m_currIsoFile);
    emit burnProgress(100);
    emit burnt(m_burnerProcess_->exitCode());
    m_currentCmd = Commands::initCmd;
    m_searchDriversTimer_->start(5000);
  }
}

void CdBurner_P::createTmpRootDir(const QStringList& filesList, const QString& dirName, const QString& baseDir)
{
  QDir tmp_iso_dir;
  tmp_iso_dir.mkpath(dirName);

  for (auto it = filesList.begin(); it != filesList.end(); ++it)
  {
    QFileInfo finfo(*it);
    QString tmp_path = finfo.absolutePath();
    auto new_path_in_tmp = tmp_path.remove(0, baseDir.length());
    tmp_iso_dir.mkpath(dirName + "/" + new_path_in_tmp);
    QFile::copy(*it, dirName + "/" + new_path_in_tmp + "/" + finfo.fileName());
  }
}

void CdBurner_P::cleanup(const QString& dirName, const QString& isoFile)
{
  QDir tmp_iso_dir(dirName);
  tmp_iso_dir.removeRecursively();
  QFile::remove(isoFile);
}
