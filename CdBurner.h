#ifndef CDBURNER_H
#define CDBURNER_H

// Qt
#include <QObject>

// Local
class CdBurner_P;

class CdBurner : public QObject
{
  Q_OBJECT
public:
  explicit CdBurner(const QString& baseDir, QObject *parent = nullptr);

public slots:
  void burnCd(const QString& dev, const QStringList& files);
  void setRootDir(const QString& rootDir);

signals:
  void listDrivers(const QHash<QString,QString>& list);
  void error(const QString& str);
  void burnProgress(int progress);
  void burnt(int result);

private:
  CdBurner_P* m_cdBurner_p;
};

#endif // CDBURNER_H
