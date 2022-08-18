#pragma once

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/android_sink.h"

#define LOGGER_NAMESPACE gobot

#define INITLOGGER LOGGER_NAMESPACE::KooLogger::Instance()->initLogger

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#define GLOGT(...) do { \
                if (LOGGER_NAMESPACE::KooLogger::Instance()->isInit()) { \
                    auto retLogger = LOGGER_NAMESPACE::KooLogger::Instance()->getLogger("GoBot"); \
                    SPDLOG_LOGGER_TRACE(retLogger, __VA_ARGS__); \
                } \
              } while(0);

#define GLOGD(...) do { \
                if (LOGGER_NAMESPACE::KooLogger::Instance()->isInit()) { \
                    auto retLogger = LOGGER_NAMESPACE::KooLogger::Instance()->getLogger("GoBot"); \
                    SPDLOG_LOGGER_DEBUG(retLogger, __VA_ARGS__); \
                } \
              } while(0);

#define GLOGI(...) do { \
                if (LOGGER_NAMESPACE::KooLogger::Instance()->isInit()) { \
                    auto retLogger = LOGGER_NAMESPACE::KooLogger::Instance()->getLogger("GoBot"); \
                    SPDLOG_LOGGER_INFO(retLogger, __VA_ARGS__); \
                } \
              } while(0);

#define GLOGW(...) do { \
                if (LOGGER_NAMESPACE::KooLogger::Instance()->isInit()) { \
                    auto retLogger = LOGGER_NAMESPACE::KooLogger::Instance()->getLogger("GoBot"); \
                    SPDLOG_LOGGER_WARN(retLogger, __VA_ARGS__); \
                } \
              } while(0);

#define GLOGE(...) do { \
                if (LOGGER_NAMESPACE::KooLogger::Instance()->isInit()) { \
                    auto retLogger = LOGGER_NAMESPACE::KooLogger::Instance()->getLogger("GoBot"); \
                    SPDLOG_LOGGER_ERROR(retLogger, __VA_ARGS__); \
                } \
              } while(0);

#define GLOGC(...) do { \
                if (LOGGER_NAMESPACE::KooLogger::Instance()->isInit()) { \
                    auto retLogger = LOGGER_NAMESPACE::KooLogger::Instance()->getLogger("GoBot"); \
                    SPDLOG_LOGGER_CRITICAL(retLogger, __VA_ARGS__); \
                } \
              } while(0);

#define SINGLETON_DECLARE_LOG(className) \
    public: \
      static className* Instance() \
      { \
        static className m_instance;\
        return &m_instance;\
      } \
    private: \
      className(const className&) = delete; \
      className& operator=(const className&) = delete;

namespace LOGGER_NAMESPACE {

class KooLogger {
    SINGLETON_DECLARE_LOG(KooLogger)

private:
    std::shared_ptr<spdlog::logger> _mLogger;
    std::vector<std::shared_ptr<spdlog::logger>> _mLoggerList;

    std::vector<spdlog::sink_ptr> _mSinks;
    spdlog::level::level_enum _mLevel;
    std::string _mPattern;

    std::mutex _m_LoggerMutex;

    bool _m_isInit;

private:
    KooLogger() : _mLevel(spdlog::level::info)
                , _m_isInit(false) {
    }

public:
    void initLogger(spdlog::level::level_enum logLevel, std::string path = "", bool isConsoleLog = false, bool isSimpleLog = true) {
        if(isConsoleLog) {
#ifdef __ANDROID__
            _mSinks.emplace_back(std::make_shared<spdlog::sinks::android_sink_mt>());
#else
            _mSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#endif
        }

        if ("" != path) {
            // TODO：检查路径是否有效
            _mSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(path));
        }
        _mLogger = std::make_shared<spdlog::logger>("Logger", begin(_mSinks), end(_mSinks));
        _mLogger->set_level(spdlog::level::err);
        if (isSimpleLog) {
            _mPattern = "%D %T:%e %5t %^%L%$: [%n] %v ; (%s:%#)";
        } else {
            _mPattern = "%t [%D %T:%e][%^%7l%$][%n] %v (%! %s:%#)";
        }
        _mLogger->set_pattern(_mPattern);
        _mLogger->flush_on(spdlog::level::err);
        spdlog::flush_every(std::chrono::seconds(5));

        _mLevel = logLevel;

        _m_isInit = true;
    }

    inline bool isInit() { return _m_isInit; }

    void uninitLogger() {
        for (auto i : _mLoggerList) {
            i->flush();
        }

        for (auto i : _mLoggerList) {
            i.reset();
        }

        _m_isInit = false;
    }

    void addLogger(std::string& loggerName) {
        if (!_m_isInit) {
            return;
        }

        //_mLogger->info("Start create logger \"{}\".", loggerName);
        SPDLOG_LOGGER_INFO(_mLogger, "Start create logger \"{}\".", loggerName);

        if(0 == _mSinks.size()) {
            // If not initialize logger sink, use default sink.
            _mSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        }
        auto retLogger = std::make_shared<spdlog::logger>(loggerName, std::begin(_mSinks), std::end(_mSinks));

        retLogger->set_level(_mLevel);
        retLogger->flush_on(spdlog::level::err);
        retLogger->set_pattern(_mPattern);

        std::lock_guard<std::mutex> lock(_m_LoggerMutex);
        _mLoggerList.emplace_back(retLogger);

        spdlog::register_logger(retLogger);
        //_mLogger->info("Create logger \"{}\" success.", loggerName);
        SPDLOG_LOGGER_INFO(_mLogger, "Create logger \"{}\" success.", loggerName);
    }

    std::shared_ptr<spdlog::logger> getLogger(std::string loggerName) {
        if (!_m_isInit) {
            return nullptr;
        }

        auto retLogger = spdlog::get(loggerName);

        if (!retLogger.get()) {
            SPDLOG_LOGGER_WARN(_mLogger, "Havn't found logger \"{}\", create it at first.", loggerName);
            addLogger(loggerName);
            retLogger = spdlog::get(loggerName);
        }

        return retLogger;
    }

};

}
