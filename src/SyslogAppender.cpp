/*
 * SyslogAppender.cpp
 *
 * Copyright 2000, LifeLine Networks BV (www.lifeline.nl). All rights reserved.
 * Copyright 2000, Bastiaan Bakker. All rights reserved.
 *
 * See the COPYING file for the terms of usage and distribution.
 */

#include "log4cpp/Config.hh"
#if LOG4CPP_HAVE_SYSLOG

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "log4cpp/SyslogAppender.hh"

namespace log4cpp {

    int SyslogAppender::toSyslogPriority(Priority::Value priority) {
        static int priorities[8] = { LOG_EMERG, LOG_ALERT, LOG_CRIT, LOG_ERR,
                                     LOG_WARNING, LOG_NOTICE, LOG_INFO, 
                                     LOG_DEBUG };
        int result;

        priority++;
        priority /= 100;

        if (priority < 0) {
            result = LOG_EMERG;
        } else if (priority > 7) {
            result = LOG_DEBUG;
        } else {
            result = priorities[priority];
        }

        return result;
    }
        

    SyslogAppender::SyslogAppender(const std::string& name, 
                                   const std::string& syslogName, 
                                   int facility) : 
        AppenderSkeleton(name),
        _syslogName(syslogName),
        _facility(facility) 
    {
        open();
    }
    
    SyslogAppender::~SyslogAppender() {
        close();
    }

    void SyslogAppender::open() {
        openlog(_syslogName.c_str(), 0, _facility);
    }

    void SyslogAppender::close() {
        ::closelog();
    }

    void SyslogAppender::_append(const LoggingEvent& event) {
        if (!_layout) {
            // XXX help! help!
            return;
        }

        const char* message = _layout->format(event).c_str();
        int priority = toSyslogPriority(event.priority);
        ::syslog(priority | _facility, message);
    }

    bool SyslogAppender::reopen() {
        close();
        open();
        return true;
    }      

    bool SyslogAppender::requiresLayout() const {
        return true;
    }

    void SyslogAppender::setLayout(Layout* layout) {
        _layout = layout;
    }

}

#endif // LOG4CPP_HAVE_SYSLOG
