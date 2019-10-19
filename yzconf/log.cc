/**************************************************************************
> File Name: log.cc
> Author:carl
> mail: carl_li510@163.com
> Created Time: 一 10/ 7 15:57:16 2019
*************************************************************************/
#include "log.h"
 #include <map>
#include <functional>  
#include <string.h>
#include <iostream>
namespace yzconf {
    
const char* LogLevel::toString( LogLevel::Level level) {
    switch( level ) {
#define XX(level) \
        case LogLevel::level:\
            return #level;
        XX(UNKNOW)
        XX(DEBUG)
        XX(INFO)
        XX(WARN)
        XX(ERROR)
        XX(FATAL)
#undef XX
    }
    return "UNKONW";    
}

class MessageFormatIterm : public LogFormater::FormatIterm {
public:
    MessageFormatIterm(const std::string& str="") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getContent();
    }
};
class LevelFormatIterm : public LogFormater::FormatIterm {
public:
    LevelFormatIterm(const std::string& str="") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << LogLevel::toString(level);
    }
};

class ElapseFormatIterm : public LogFormater::FormatIterm {
public:
    ElapseFormatIterm(const std::string& str="") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getElapse();
    }
};

class NameFormatIterm : public LogFormater::FormatIterm {
public:
    NameFormatIterm(const std::string& str="") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << logger->getName();
    }
};

class ThreadIdFormatIterm : public LogFormater::FormatIterm {
public:
    ThreadIdFormatIterm(const std::string& str="") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getThreadId();
    }
};
class FiberIdFormatIterm : public LogFormater::FormatIterm {
public:
    FiberIdFormatIterm(const std::string& str="") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFiberId();
    }
};
class FileNameFormatIterm : public LogFormater::FormatIterm {
public:
    FileNameFormatIterm(const std::string& str="") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getFile();
    }
};
class NewLineFormatIterm : public LogFormater::FormatIterm {
public:
    NewLineFormatIterm(const std::string& str="") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << std::endl;
    }
};

class LineFormatIterm : public LogFormater::FormatIterm {
public:
    LineFormatIterm(const std::string& str="") {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << event->getLine();
    }
};

class DateTimeFormatIterm : public LogFormater::FormatIterm {
public:
    DateTimeFormatIterm(const std::string& str) 
        :m_format(str) {
        if( str.empty() ) {
            m_format = "%Y%m%d %H:%M:%S";
        }
    }
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buff[64];
        strftime(buff, sizeof(buff), m_format.c_str(), &tm);
        os << buff;
    }
private:
    std::string m_format;
};
class StringFormatIterm : public LogFormater::FormatIterm {
public:
    StringFormatIterm(const std::string& str) 
        :m_string(str) {
    }
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << m_string;
    }
private:
    std::string m_string;
};

void Logger::log(LogLevel::Level level, LogEvent::ptr event) {
    if( level > m_level ) {
        auto self = shared_from_this();
        for( auto& i: m_appenders ) {
            i->log(self, level, event);
        }
    }
}
void Logger::debug(LogEvent::ptr event) {
    log(LogLevel::DEBUG, event);
}
void Logger::info(LogEvent::ptr event) {
    log(LogLevel::INFO, event);
}
void Logger::warn(LogEvent::ptr event) {  
    log(LogLevel::WARN, event);
}
void Logger::error(LogEvent::ptr event) {
    log(LogLevel::ERROR, event);
}
void Logger::fatal(LogEvent::ptr event) {
    log(LogLevel::FATAL, event);
}

void Logger::addAppender(LogAppender::ptr appender) {
    m_appenders.push_back(appender);
}
void Logger::delAppender(LogAppender::ptr appender) {
    for( auto it = m_appenders.begin();
            it != m_appenders.end();
            it++ ) {
        if( *it == appender ) {
            m_appenders.erase(it);
            break;
        }
    }
}

void StdoutLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)  {
    if( level >= m_level ) {
        std::cout << m_formater->format(logger, level, event);
    }
}
FileLogAppender::FileLogAppender( const std::string& filename) 
    :m_filename(filename) {
    reopen();
}
void FileLogAppender::log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    if( level >= m_level) {
        reopen();
        m_filestream << m_formater->format(logger, level, event);
    }
}

bool FileLogAppender::reopen() {
    if( m_filestream ) {
        m_filestream.close();
    }
    m_filestream.open(m_filename);
    return !!m_filestream;
} 

LogFormater::LogFormater(const std::string& pattern) 
    :m_pattern(pattern) {
    init();
}
void LogFormater::init() {
    //str, format, type
    std::vector<std::tuple<std::string, std::string, int>> vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); ++i) {
        if(m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if((i + 1) < m_pattern.size()) {
            if(m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()) {
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                    && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0) {
                if(m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    //std::cout << "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if(fmt_status == 1) {
                if(m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    //std::cout << "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if(fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }

    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string, std::function<FormatIterm::ptr(const std::string& str)> > s_format_iterms = {
    #define XX(_Str, _C)  {#_Str, [](const std::string& fmt){ return FormatIterm::ptr(new _C(fmt)); }}
        XX(m, MessageFormatIterm),
        XX(p, LevelFormatIterm),
        XX(r, ElapseFormatIterm),
        XX(c, NameFormatIterm),
        XX(t, ThreadIdFormatIterm),
        XX(n, NewLineFormatIterm),
        XX(d, DateTimeFormatIterm),
        XX(f, FileNameFormatIterm),
        XX(l, LineFormatIterm),
    #undef XX
    };
    //%p -- level
    //%m -- 消息体
    //%r -- 启动后的时间
    //%c -- 日志名称
    //%t -- 线程id
    //%n -- 回车换行
    //%d -- 时间 
    //%f -- 文件名
    //%l -- 行号
    for( auto& i : vec ) {
        if(  std::get<2>(i) == 0 ) {
            m_iterms.push_back(FormatIterm::ptr(new StringFormatIterm(std::get<0>(i))));
        } else {
            auto it = s_format_iterms.find(std::get<0>(i));
            if( it == s_format_iterms.end() ) {
                m_iterms.push_back(FormatIterm::ptr(new StringFormatIterm("<error_format> %" + std::get<0>(i))));
            } else {
                m_iterms.push_back(FormatIterm::ptr(it->second(std::get<1>(i))));
            }
        }
        std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;  
    }
}

std::string LogFormater::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) {
    std::stringstream ss;
    for( auto& it : m_iterms ) {
        it->format(ss, logger, level, event);
    }
    return ss.str();
}

}
