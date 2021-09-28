// Local
#include "CdBurner.h"
#include "CdBurner_p.h"

CdBurner::CdBurner(const QString& baseDir, QObject *parent)
  : QObject(parent)
  , m_cdBurner_p(new CdBurner_P(baseDir, this))
{
  connect(m_cdBurner_p, &CdBurner_P::error, this, &CdBurner::error);
  connect(m_cdBurner_p, &CdBurner_P::listDrivers, this, &CdBurner::listDrivers);
  connect(m_cdBurner_p, &CdBurner_P::burnProgress, this, &CdBurner::burnProgress);
  connect(m_cdBurner_p, &CdBurner_P::burnt, this, &CdBurner::burnt);
}

void CdBurner::burnCd(const QString& dev, const QStringList& files)
{
  m_cdBurner_p->burnCd(dev, files);
}

void CdBurner::setRootDir(const QString& rootDir)
{
  m_cdBurner_p->setRootDir(rootDir);
}
