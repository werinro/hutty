#ifndef __WLR__LOGER__H
#define __WLR__LOGER__H

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <map>

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <map>
#include <sys/time.h>
#include "constant.h"


#define NEW_LINE '\n'
#define W_DEBUG 0
#define W_INFO 1
#define W_WARN 2
#define W_ERROR 3

#ifndef LOG_LEVEL
#define	LOG_LEVEL W_INFO
#endif

#ifndef LOG_DIR
#define LOG_DIR "./logs/"
#endif

#define LOG_FORMAT(level, file_name, format, ...) wlr::Logger::log(level, wlr::OutFileHandler::getHandler(file_name), __FILE__, __LINE__, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) LOG_FORMAT(wlr::Logger::DEBUG, "debug.log", format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) LOG_FORMAT(wlr::Logger::INFO, "info.log", format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) LOG_FORMAT(wlr::Logger::WARN, "warn.log", format, ##__VA_ARGS__)
#define LOG_ERROR(format, ...) LOG_FORMAT(wlr::Logger::ERROR, "error.log", format, ##__VA_ARGS__)
//#define LOG_INFO(format, ...) wlr::Logger::log(wlr::Logger::INFO, __FILE__, __LINE__, format, ##__VA_ARGS__);

#define LOG_STREAM(level, file_name) wlr::Logger(level, wlr::OutFileHandler::getHandler(file_name), __FILE__, __LINE__).to_stream()
#define LOG_DEBUG_STREAM LOG_STREAM(wlr::Logger::DEBUG, "debug.log")
#define LOG_INFO_STREAM LOG_STREAM(wlr::Logger::INFO, "info.log")
#define LOG_WARN_STREAM LOG_STREAM(wlr::Logger::WARN, "warn.log")
#define LOG_ERROR_STREAM LOG_STREAM(wlr::Logger::ERROR, "error.log")

#define LOG(name, level, file_name) \
	wlr::Logger name(wlr::Logger::level, wlr::OutFileHandler::getHandler(file_name), __FILE__, __LINE__)

namespace wlr
{


class LoggerHandler;
class Logger
{
public:
    enum Level
    {
        DEBUG,
        INFO,
        WARN,
        ERROR,
    };

    Logger(wlr::Logger::Level level, wlr::LoggerHandler* out_handler, const char* file, int line);
	
	template<class T>
	friend wlr::Logger& operator<< (wlr::Logger&, T);

    //void* operator new (size_t) = delete;
    ~Logger();

private:
    //void* operator new[] (size_t){return nullptr;}
    //void operator delete (void*){}
public:
    void console();
    // console 依赖于 out, 需优先执行此函数
    void out();

	std::string ymdstr();
	std::string content();
	wlr::Logger::Level level();
    std::stringstream& to_stream();
	static std::string getLevelString(wlr::Logger::Level level, bool color_code = true);
    static void log(wlr::Logger::Level level, wlr::LoggerHandler* out_handler, const char* file, int line, const char* format, ...);
private:
	std::string m_ymd_str;
    std::string m_content;
	wlr::Logger::Level m_level;
    std::stringstream m_str_stream;
	wlr::LoggerHandler* m_out_handler = NULL;
    static wlr::LoggerHandler* m_level_handler[4];
    
    void* operator new (size_t){return NULL;}
};

template<class T>
wlr::Logger& operator<< (wlr::Logger& log, T t)
{
    log.m_str_stream << t;
	return log;
}


class LoggerHandler
{
public:
    virtual ~LoggerHandler();
    virtual void execution(Logger& logger) = 0;
	void handler(Logger& logger);
};

class DebugHandler : public LoggerHandler
{
public:
    virtual void execution(Logger& logger);
};

class InfoHandler : public LoggerHandler
{
public:
    virtual void execution(Logger& logger);
};

class WarnHandler : public LoggerHandler
{
public:
    virtual void execution(Logger& logger);
};

class ErrorHandler : public LoggerHandler
{
public:
    virtual void execution(Logger& logger);
};


class OutFileHandler : public LoggerHandler
{
public:
    virtual void execution(Logger& logger);
	static wlr::OutFileHandler* getHandler(std::string file_name, bool out_level = true);

private:
	OutFileHandler(std::string file_name, bool out_level = true);
	
	bool m_out_level;
	std::string m_file_name;
	static std::map<std::string, wlr::OutFileHandler*> init();
	static std::map<std::string, wlr::OutFileHandler*> s_file_handler_cache;
};


}

#endif
