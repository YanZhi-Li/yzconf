/*************************************************************************
	> File Name: log.h
	> Author: carl
	> Mail: carl_li510@163.com 
	> Created Time: 一 10/ 7 14:52:10 2019
 ************************************************************************/

#ifndef __YZCONF_LOG_H_
#define __YZCONF_LOG_H_

#include <iostream>
#include <memory>
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <list>
#include <vector>
#include <fstream>
#include <map>
#include <sstream>
#include <stdarg.h>

#define YZCONF_LOG_LEVEL(_logger, _level) \
    if( _logger->getLevel() <= _level )  \
        yzconf::LogEventWrap(yzconf::LogEvent::ptr(new yzconf::LogEvent(_logger, _level,\
                         __FILE__, __LINE__, 0, yzconf::GetThreadId(), yzconf::GetFiberId(), time(0)))).getSS()
#define YZCONF_LOG_DEBUG(_logger )  YZCONF_LOG_LEVEL(_logger, yzconf::LogLevel::DEBUG)
#define YZCONF_LOG_INFO(_logger )   YZCONF_LOG_LEVEL(_logger, yzconf::LogLevel::INFO)
#define YZCONF_LOG_WARN(_logger )   YZCONF_LOG_LEVEL(_logger, yzconf::LogLevel::WARN)
#define YZCONF_LOG_ERROR(_logger )  YZCONF_LOG_LEVEL(_logger, yzconf::LogLevel::ERROR)
#define YZCONF_LOG_FATAL(_logger )  YZCONF_LOG_LEVEL(_logger, yzconf::LogLevel::FATAL)

#define YZCONF_LOG_FMT_LEVEL(_logger, _level, _fmt, ...) \
    if( _logger->getLevel() <= _level )  \
        yzconf::LogEventWrap(yzconf::LogEvent::ptr(new yzconf::LogEvent(_logger, yzconf::LogLevel::DEBUG,\
                         __FILE__, __LINE__, 0, yzconf::GetThreadId(), yzconf::GetFiberId(), time(0)))).getEvent()->format(_fmt, __VA_ARGS__)
#define YZCONF_LOG_FMT_DEBUG(_logger, _fmt, ...) YZCONF_LOG_FMT_LEVEL(_logger, yzconf::LogLevel::DEBUG, _fmt, __VA_ARGS__)
namespace yzconf {

class Logger;

//日志级别
class LogLevel {
public:
    enum Level {
        UNKNOW= 0,
        DEBUG = 1,
        INFO  = 2,
        WARN  = 3,
        ERROR = 4,
        FATAL = 5,
    };
    static const char* toString( LogLevel::Level level); 
};
//日志事件
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(std::shared_ptr<Logger> logger,
            LogLevel::Level level,
            const char* file,
            int line,
            uint32_t elapse,
            pthread_t thread_id,
            uint32_t fiber_id,
            uint64_t time,
            const std::string& thread_name = "test")
        :m_file(file)
        ,m_line(line)
        ,m_elapse(elapse)
        ,m_threadId(thread_id)
        ,m_fiberId(fiber_id)
        ,m_time(time)
        ,m_level(level) 
        ,m_threadname(thread_name)
        ,m_logger(logger) 
        {
        
    }
    virtual ~LogEvent() {}

    const char* getFile() const { return m_file; }
    uint32_t getLine() const { return m_line; }
    uint32_t getElapse() const { return m_elapse;}
    pthread_t getThreadId() const { return m_threadId; }
    uint32_t getFiberId() const { return m_fiberId; }
    uint64_t getTime() const { return m_time; }
    std::string getContent() const { return m_ss.str(); }
    std::stringstream& getSS() { return m_ss; };
    LogLevel::Level getLevel() const { return m_level; }
    std::shared_ptr<Logger>& getLogger() { return m_logger; }
    std::string getThreadName() { return m_threadname; }

    void format(const char* fmt, ...);
    void format(const char* fmt, va_list val);
private:
    const char* m_file = nullptr;   //文件名
    int m_line = 0;            //行号:
    uint32_t m_elapse = 0;          //程序启动到现在时间
    pthread_t m_threadId = 0;        //线程id
    uint32_t m_fiberId = 0;         //协程id
    uint64_t m_time = 0;            //时间戳
    LogLevel::Level m_level;
    std::string m_threadname;
    std::stringstream m_ss;
    std::shared_ptr<Logger> m_logger;
    
};

class LogEventWrap {
public:
    typedef std::shared_ptr<LogEventWrap> ptr;
    LogEventWrap(LogEvent::ptr e);
    ~LogEventWrap();

    std::stringstream& getSS();
    LogEvent::ptr& getEvent() { return m_event; }
private:
    LogEvent::ptr m_event;
};

//日志格式
class LogFormater {
public:
    typedef std::shared_ptr<LogFormater> ptr;
    LogFormater(const std::string& pattern);

    void init();
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
public:
    class FormatIterm {
    public:
        typedef std::shared_ptr<FormatIterm> ptr;
        FormatIterm(const std::string& str="") { };
        virtual ~FormatIterm() {}
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };
private:
    std::string m_pattern;
    std::vector<FormatIterm::ptr> m_iterms;
};
//日志输出地
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    void setFormater( LogFormater::ptr val ) { m_formater = val;} 
    LogFormater::ptr getFormater() { return m_formater; }

    void setLevel( LogLevel::Level level) { m_level = level;}
    LogLevel::Level getLevel() const { return m_level; }
protected:
    LogLevel::Level m_level;
    LogFormater::ptr m_formater;
};


//日志器
class Logger : public std::enable_shared_from_this<Logger> {
public:
    typedef std::shared_ptr<Logger> ptr;
    Logger(const std::string& name="root")
        :m_name( name ) 
        ,m_level(LogLevel::DEBUG) {
        
    }

    void log(LogLevel::Level level, LogEvent::ptr event);
    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel() const { return m_level; }
    void setLevel( LogLevel::Level level) { m_level = level; }
    const std::string getName() const { return m_name; }
private:
    std::string m_name;
    LogLevel::Level m_level;
    std::list<LogAppender::ptr> m_appenders;
};

class StdoutLogAppender : public LogAppender {
public:
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

class FileLogAppender : public LogAppender {
public:
    FileLogAppender( const std::string& filename);
    void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;

    bool reopen();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};

}


#endif //__YZCONF_LOG_H_
