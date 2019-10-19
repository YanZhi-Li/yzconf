#include <iostream>
#include "../yzconf/log.h"
#include "../yzconf/util.h"

int main(const int argc, const char *argv[])
{
    yzconf::Logger::ptr logger(new yzconf::Logger::Logger);
    yzconf::LogAppender::ptr stdappender(new yzconf::StdoutLogAppender);
    stdappender->setFormater(yzconf::LogFormater::ptr(new yzconf::LogFormater("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n")));
    logger->addAppender(stdappender);

    yzconf::LogAppender::ptr fileappender(new yzconf::FileLogAppender("./log.txt"));
    fileappender->setFormater(yzconf::LogFormater::ptr(new yzconf::LogFormater("%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T[%p]%T[%c]%T%f:%l%T%m%n")));
    fileappender->setLevel(yzconf::LogLevel::ERROR);
    logger->addAppender(fileappender);

    
    //yzconf::LogEvent::ptr event(new yzconf::LogEvent(logger, yzconf::LogLevel::DEBUG , __FILE__, __LINE__, 0, yzconf::GetThreadId(), 2, time(0), "test"));
    

    YZCONF_LOG_DEBUG(logger) << "hello, world";
    YZCONF_LOG_ERROR(logger) << "test error";
    YZCONF_LOG_FMT_DEBUG(logger, "ni hao %s", "lilei");
    //event->getSS() << "hello";
    //event->getSS() << ",world";
    //logger->log(yzconf::LogLevel::ERROR, event);
    
    //std::cout << "hello, world\n";
    return 0;
}
