#include <iostream>
#include "../yzconf/log.h"
#include "../yzconf/util.h"

int main(const int argc, const char *argv[])
{
    yzconf::Logger::ptr logger(new yzconf::Logger::Logger);
    yzconf::LogAppender::ptr stdappender(new yzconf::StdoutLogAppender);
    stdappender->setFormater(yzconf::LogFormater::ptr(new yzconf::LogFormater("%p %t [%d{%Y%m%d %H:%M:%S}] <%f:%l> =>%m")));
    logger->addAppender(stdappender);
    
    yzconf::LogEvent::ptr event(new yzconf::LogEvent(logger, yzconf::LogLevel::DEBUG , __FILE__, __LINE__, 0, yzconf::GetThreadId(), 2, time(0), "test"));
    logger->log(yzconf::LogLevel::ERROR, event);

    YZCONF_LOG_DEBUG(logger) << "hello, world\n";
    
    //std::cout << "hello, world\n";
    return 0;
}
