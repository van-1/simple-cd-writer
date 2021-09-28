// Qt
#include <QTextStream>
#include <QDateTime>

// Local
#include "Logger.h"

static void logMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
  if (Logger::instance())
    Logger::instance()->logMessageOutput(type, context, msg);
}

std::unique_ptr<Logger> Logger::m_instance;
std::once_flag Logger::m_onceFlag;

Logger* Logger::instance()
{
  std::call_once(Logger::m_onceFlag,[] (){
    m_instance.reset(new Logger);
    if (m_instance)
      qInstallMessageHandler(logMessage);
  });
  return m_instance.get();
}

void Logger::logMessageOutput(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
  QTextStream cout(stdout);
    auto now = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss");
    switch (type) {
      case QtDebugMsg:
        if (m_settings.full_log)
          cout << now << " Debug " << msg << "\n";
        break;
      case QtInfoMsg:
        cout << now << " Info " << msg << "\n";
        break;
      case QtWarningMsg:
        cout << now << " Warning " << msg << "\n";
        break;
      case QtCriticalMsg:
        cout << now << " Critical " << msg << "\n";
        break;
      case QtFatalMsg:
        cout << now << "Fatal " << msg << "\n";
        break;
    }
    cout.flush();
}

void Logger::setSettings(const Settings& settings)
{
  m_settings = settings;
}
