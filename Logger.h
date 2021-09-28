#ifndef LOGGER_H
#define LOGGER_H

// STL
#include <memory>
#include <mutex>

// Qt
#include <QObject>

// Local
#include <Settings.h>

class Logger
{
public:
  static Logger* instance();
  void logMessageOutput(QtMsgType type, const QMessageLogContext&, const QString& msg);
  void setSettings(const Settings& settings);

private:
  Logger() = default;
  Logger(const Logger&) = delete;
  Logger& operator=(const Logger&) = delete;

  static std::unique_ptr<Logger> m_instance;
  static std::once_flag m_onceFlag;
  Settings m_settings;
};

#endif // LOGGER_H
