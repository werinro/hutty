#ifndef __WLR__CONFIG__H
#define __WLR__CONFIG__H


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>

#include "../structure/maps.h"
#include "logger.h"


#ifndef CONFIG_FILE_PATH
#define CONFIG_FILE_PATH "./config.ini"
#endif

namespace wlr{


class Configure
{
//private:
public:
    ~Configure();

public:
//#ifdef CONFIG_FILE_PATH
    Configure(std::string filePath = CONFIG_FILE_PATH);
    static void loadConfig(std::string filePath = CONFIG_FILE_PATH);
    static void writeConfig(std::string filePath = CONFIG_FILE_PATH, std::string module = "");
//#elif
//#endif

    static std::string getValue(std::string key, std::string default_value = "", std::string module = "default");
    static bool setConfig(std::string key, std::string value, std::string module = "default");
private:
    static std::map<std::string, std::map<std::string, std::string> > s_configCache;
};

}

#endif
