#ifndef CDBURNER_P_H
#define CDBURNER_P_H

// Qt
#include <QObject>
#include <QHash>
class QTimer;

// Local
class ProcessWrapper;

class CdBurner_P : public QObject
{
  Q_OBJECT

  enum Commands
  {
    initCmd,
    findDriversCmd,
    burnCdCmd
  };

public:
  explicit CdBurner_P(const QString& base_dir, QObject* parent = nullptr);

public slots:
  void burnCd(const QString& dev, const QStringList& files);
  void setRootDir(const QString& rootDir);

signals:
  void listDrivers(const QHash<QString,QString>& list);
  void error(const QString& str);
  void burnProgress(int progress);
  void burnt(int result);

private slots:
  void readStdOut(const QString& str);
  void searchDrivers();
  void processFinished();
  void createTmpRootDir(const QStringList& filesList, const QString& dirName, const QString& baseDir);
  void cleanup(const QString& dirName, const QString& isoFile);

private:
  ProcessWrapper* m_burnerProcess_;
  ProcessWrapper* m_findDevicesProcess_;
  QTimer* m_searchDriversTimer_;
  Commands m_currentCmd;
  QHash<QString, QString> m_drivers;
  QString m_baseDir;
  QString m_currIsoFile;
  QString m_currTmpIsoDir;
};

#endif // CDBURNER_P_H
