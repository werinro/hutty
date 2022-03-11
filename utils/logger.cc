#include "logger.h"


wlr::Logger::Logger(wlr::Logger::Level level, wlr::LoggerHandler* out_handler, const char* file, int line)
    : m_level(level)
	, m_out_handler(out_handler)
{
    this->m_str_stream << "(" << file << "-" << line << ")-> ";
}

wlr::Logger::~Logger()
{
    wlr::Logger::m_level_handler[this->m_level]->execution(*this);
}

void wlr::Logger::console()
{
    printf((wlr::Logger::getLevelString(this->m_level) + this->m_content).c_str());
}

void wlr::Logger::out()
{
	struct timeval tval;
	gettimeofday(&tval, NULL);
    //struct tm* t = gmtime(&tval.tv_sec);
    struct tm* t = localtime(&tval.tv_sec);
	int msec = static_cast<int>(tval.tv_usec / 1000);

    std::stringstream time_str;
    if (t)
    {
        time_str << "[" << t->tm_year + 1900 << "-" << t->tm_mon + 1 << "-" << t->tm_mday;
        this->m_ymd_str = time_str.str().substr(1);
        time_str << " " << t->tm_hour << ":" << t->tm_min << ":" << t->tm_sec << "." << msec << "]";
    }
    
    // time_str << " [p=" << wlr::processId() << ":t=" << wlr::threadId() <<  "] ";
    time_str << " [pid=" << wlr::threadId() <<  "] ";
    this->m_content.append(this->m_str_stream.str());
    this->m_content = time_str.str() + this->m_content;
	if (this->m_out_handler) this->m_out_handler->execution(*this);
}

std::string wlr::Logger::ymdstr()
{ return this->m_ymd_str; }
    
std::string wlr::Logger::content()
{ return this->m_content; }

wlr::Logger::Level wlr::Logger::level()
{ return this->m_level; }

std::stringstream& wlr::Logger::to_stream()
{ return this->m_str_stream; }

std::string wlr::Logger::getLevelString(wlr::Logger::Level level, bool color_code)
{
	switch(level)
    {
#define TO_PRINT(lev, str, str_) \
    case lev: \
	if (color_code) \
		return std::string(str); \
	else \
		return std::string(str_);

    TO_PRINT(DEBUG, "\e[49;32m[DEBUG]\e[0m ", "[DEBUG] ")
    TO_PRINT(INFO, "\e[49;34m[INFO]\e[0m ", "[INFO] ")
    TO_PRINT(WARN, "\e[49;33m[WARN]\e[0m ", "[WARN] ")
    TO_PRINT(ERROR, "\e[49;31m[ERROR]\e[0m ", "[ERROR] ")

#undef TO_PRINT
    }
	return "[.]";
}

void wlr::Logger::log(Logger::Level level, wlr::LoggerHandler* out_handler, const char* file, int line, const char* format, ...)
{
    va_list args;
    char* buff = NULL;
    Logger logger(level, out_handler, file, line);

    va_start(args, format);
    //va_arg(args, char);
    int len = vasprintf(&buff, format, args);
    
    if(len != -1){
        //printf(std::string(buff, len).c_str());
        logger.m_str_stream << std::string(buff, len);
        free(buff);
    }
    va_end(args);
}

wlr::LoggerHandler* wlr::Logger::m_level_handler[4] = {new wlr::DebugHandler, new wlr::InfoHandler, new wlr::WarnHandler, new wlr::ErrorHandler};


wlr::LoggerHandler::~LoggerHandler(){}

void wlr::LoggerHandler::handler(Logger& logger)
{
#ifndef LOG_NO_OUT
	logger.out(); 
#endif 
#ifndef LOG_NO_CONSOLE 
	logger.console(); 
#endif 
}

void wlr::DebugHandler::execution(Logger& logger)
{
#if LOG_LEVEL <= W_DEBUG
    this->handler(logger);
#endif
}

void wlr::InfoHandler::execution(Logger& logger)
{
#if LOG_LEVEL <= W_INFO
    this->handler(logger);
#endif
}

void wlr::WarnHandler::execution(Logger& logger)
{
#if LOG_LEVEL <= W_WARN
    this->handler(logger);
#endif
}

void wlr::ErrorHandler::execution(Logger& logger)
{
#if LOG_LEVEL <= W_ERROR
    this->handler(logger);
#endif
}


wlr::OutFileHandler::OutFileHandler(std::string file_name, bool out_level)
	: m_file_name(file_name)
	, m_out_level(out_level)
{}

void wlr::OutFileHandler::execution(Logger& logger)
{
    std::string file_path(LOG_DIR);
    file_path.append("/");
    file_path.append(logger.ymdstr());
    system((std::string("test -e ") + file_path + " || mkdir -p " + file_path + ";").c_str());
    file_path.append("/");
    file_path.append(this->m_file_name);
	/*FILE* file = fopen(file_path.c_str(), "ab+");
    if (file)
    {
		std::string out_str;
		if (this->m_out_level)
            out_str += wlr::Logger::getLevelString(logger.level(), false);

		out_str += logger.content();
		fwrite(out_str.c_str(), 1, out_str.size(), file);
		fclose(file);
	}*/

    std::ofstream out(file_path.c_str(), std::ios::app | std::ios::out);
    if (out)
    {
		if (this->m_out_level)
			out << wlr::Logger::getLevelString(logger.level(), false);
        out << logger.content();
    }
    else
    {
        printf("open log file %s fail, unable write log.\n", file_path.c_str());
    }

    out.close();
}

wlr::OutFileHandler* wlr::OutFileHandler::getHandler(std::string file_name, bool out_level)
{
	auto iter = wlr::OutFileHandler::s_file_handler_cache.find(file_name);
	if (iter == wlr::OutFileHandler::s_file_handler_cache.end()) {
		iter = s_file_handler_cache.insert(std::pair<std::string, wlr::OutFileHandler*>(file_name, new wlr::OutFileHandler(file_name, out_level))).first;
	}
	return (*iter).second;
}

std::map<std::string, wlr::OutFileHandler*> wlr::OutFileHandler::init()
{
	std::map<std::string, wlr::OutFileHandler*> s_file_handler_cache;
	s_file_handler_cache.insert(std::pair<std::string, wlr::OutFileHandler*>("debug.log", new wlr::OutFileHandler("debug.log", false)));
	s_file_handler_cache.insert(std::pair<std::string, wlr::OutFileHandler*>("info.log", new wlr::OutFileHandler("info.log", false)));
	s_file_handler_cache.insert(std::pair<std::string, wlr::OutFileHandler*>("warn.log", new wlr::OutFileHandler("warn.log", false)));
	s_file_handler_cache.insert(std::pair<std::string, wlr::OutFileHandler*>("error.log", new wlr::OutFileHandler("error.log", false)));
	return s_file_handler_cache;
}

std::map<std::string, wlr::OutFileHandler*>  wlr::OutFileHandler::s_file_handler_cache = wlr::OutFileHandler::init();

