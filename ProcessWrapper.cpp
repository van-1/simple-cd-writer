#include <QProcess>

#include "ProcessWrapper.h"

ProcessWrapper::ProcessWrapper(QString programPath, QStringList arguments, QObject *parent)
  : QObject(parent)
  , m_process(new QProcess(this))
{
  connect(m_process, &QProcess::errorOccurred, this, &ProcessWrapper::error);
  connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
          this, &ProcessWrapper::exited);
  connect(m_process, &QProcess::readyReadStandardError, this, &ProcessWrapper::readStandardError);
  connect(m_process, &QProcess::readyReadStandardOutput, this, &ProcessWrapper::readStandardOutput);
  connect(m_process, &QProcess::stateChanged, this, &ProcessWrapper::state);

  if (arguments.isEmpty())
  {
    if (!programPath.isEmpty())
      m_process->start(programPath);
  }
  else
  {
    if (!programPath.isEmpty())
      m_process->start(programPath, arguments);
  }
}

bool ProcessWrapper::running() const
{
  return (m_process->state() == QProcess::Running);
}

int ProcessWrapper::exitCode() const
{
  return m_process->exitCode();
}

void ProcessWrapper::readStandardError()
{
  emit stdError(m_process->readAllStandardError());
}

void ProcessWrapper::readStandardOutput()
{
  emit stdOut(m_process->readAllStandardOutput());
}

void ProcessWrapper::start(QString programPath, QStringList arguments)
{
  if (!programPath.isEmpty())
    m_process->start(programPath, arguments);
  else
    emit stdError("program path is empty");
}

void ProcessWrapper::start(QString programPath)
{
  if (!programPath.isEmpty())
    m_process->start(programPath);
  else
    emit stdError("program path is empty");
}

void ProcessWrapper::kill()
{
  m_process->kill();
}
