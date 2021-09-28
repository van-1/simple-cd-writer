#ifndef PROCESSWRAPPER_H
#define PROCESSWRAPPER_H

// Qt
#include <QObject>
#include <QStringList>
#include <QProcess>

class ProcessWrapper : public QObject
{
  Q_OBJECT
public:
  explicit ProcessWrapper(QString programPath = "", QStringList arguments = QStringList(), QObject* parent = nullptr);
  bool running() const;
  int exitCode() const;

signals:
  void stdOut(const QString& output);
  void stdError(const QString& error);
  void error(QProcess::ProcessError error);
  void exited(int exitCode, QProcess::ExitStatus exitStatus);
  void state(QProcess::ProcessState newState);

public slots:
  void start(QString programPath, QStringList arguments);
  void start(QString programPath);
  void kill();

private slots:
  void readStandardError();
  void readStandardOutput();

private:
  QProcess* m_process;
};

#endif // PROCESSWRAPPER_H
