#include "config.h"


wlr::Configure::~Configure()
{
    LOG_DEBUG("wlr::Configure::~Configure()\n");
}

wlr::Configure::Configure(std::string filePath)
{
    Configure::loadConfig(filePath);
}

void wlr::Configure::loadConfig(std::string filePath)
{
    FILE* file = fopen(filePath.c_str(), "rb");
    if (file)
    {
        char c;
		char buf[2048], chars[1024 << 10];
		bool notes = 0, isModule = 0, isStr = 0;
		int bufIndex, bufSize = 0, flag = 0, index = 0;

#define W_APPEND_ENTITY \
	iter = wlr::Configure::s_configCache.insert(wlr::toPair(moduleName, wlr::stringMap())).first; \

		std::string moduleName("default");
		auto iter = wlr::Configure::s_configCache.find(moduleName);
		if (iter == wlr::Configure::s_configCache.end()) {
			W_APPEND_ENTITY
		}
		
		LOG_DEBUG("file name = %s\n", filePath.c_str());
		while((bufSize = fread(buf, 1, 2048, file)) > 0)
		{
			for(bufIndex = 0; bufIndex < bufSize; bufIndex++)
			{
				c = buf[bufIndex];
				if (notes)
				{
					if (c != '\n')
						continue;
					else 
						notes = false;
				}
				else if (isModule)
				{
					if (c == ']') {
						moduleName = std::string(chars, index);
						W_APPEND_ENTITY
                        isModule = false;
                        index = 0;	
					} 
					else if (c == ' ') continue;
					else {
						chars[index++] = c;
					}
				} 
				else if (isStr) 
				{
					if (c == 39 || c == '"') {
						isStr = false;
					} else {
						chars[index++] = c;
					}
				} 
				else
				{
					switch(c)
					{
						case ' ':
							break;
						case '[':
							if (!index) isModule = true;
							else chars[index++] = c;
							break;
						case 39:
						case '"':
							if (index < 1 || chars[index - 1] != '\\') isStr = true;
							else chars[index++] = c;
							break;
						case '#':
						case ';':
							if (!index) notes = true;
							else chars[index++] = c;
							break;
						case '\n':
							if (flag)
							{
								std::string key(chars, flag);
								std::string value(&chars[flag], index - flag);
								(*iter).second.insert(std::pair<std::string, std::string>(key, value));
							}
							index = 0;
							flag = 0;
							break;
						case '=':
							if (!flag && index) flag = index;
							break;
						default:
							chars[index++] = c;
					}
				}
			}
		}
    }
	else
    {
        LOG_WARN("open file %s fail.\n", filePath.c_str());
		return;
    }
    
    fclose(file);

#undef W_APPEND_ENTITY

    LOG_DEBUG("load configure done.\n");
    //for (auto iter = wlr::Configure::s_configCache.begin(); iter != wlr::Configure::s_configCache.end(); iter++)
    for (auto i = wlr::Configure::s_configCache.begin(); i != wlr::Configure::s_configCache.end(); i++)
    {
		LOG_DEBUG("[%s]\n", (*i).first.c_str());
		for (auto mi : (*i).second)
		{
			LOG_DEBUG("key = %s : value = %s\n", mi.first.c_str(), mi.second.c_str());
		}
    }
}

void wlr::Configure::writeConfig(std::string filePath, std::string module)
{
    std::string tips("#可使用#或;开头注释掉整行. 不支持参数带空格, 读取时会过滤掉空格.\n#格式:\n"
						";[模块名]\n"
						";key=value\n" 
						";key =  value\n" 
						";以换行结尾.\n\n");

	std::map<std::string, std::map<std::string, std::string> > confMap;
	if (module.empty())
	{
		for (auto iter : wlr::Configure::s_configCache)
			confMap.insert(wlr::toPair(iter.first, iter.second));
	}
	else
	{
		auto mapIter = wlr::Configure::s_configCache.find(module);
		if (mapIter != wlr::Configure::s_configCache.end())
			confMap.insert(wlr::toPair((*mapIter).first, (*mapIter).second));
		else {
			LOG_WARN("not find config module %s.\n", module.c_str());
			return;
		}
	}

    FILE* f = fopen(filePath.c_str(), "wb");
    if (f)
    {
        fwrite(tips.c_str(), 1, tips.size(), f);
		for (auto iter = confMap.begin(); iter != confMap.end(); iter++)
		{
			std::string moduleName("[" + (*iter).first + "]\n");
			fwrite(moduleName.c_str(), sizeof(char), moduleName.length(), f);
			for (auto mapIter : (*iter).second)
			{
				std::string conf(mapIter.first + "=" + mapIter.second + "\n");
				fwrite(conf.c_str(), 1, conf.size(), f);
			}
			fwrite("\n", 1, 1, f);
		}

        fclose(f);
		LOG_INFO("write config module %s to %s file done.\n", module.c_str(), filePath.c_str());
    } else {
        LOG_INFO("open %s file fail, write config stop.\n", filePath.c_str());
    }
}

std::string wlr::Configure::getValue(std::string key, std::string default_value, std::string module)
{
    std::map<std::string, std::map<std::string, std::string> >::iterator iter;
    if ((iter = wlr::Configure::s_configCache.find(module)) != wlr::Configure::s_configCache.end()) {
		std::map<std::string, std::string> map = (*iter).second;
		std::map<std::string, std::string>::iterator mapIter = map.find(key);
		if (mapIter != map.end())
			return (*mapIter).second;
    }

    return default_value;
}

bool wlr::Configure::setConfig(std::string key, std::string value, std::string module)
{
	auto iter = wlr::Configure::s_configCache.find(module);
	if (iter == wlr::Configure::s_configCache.end())
	{
		wlr::Configure::s_configCache.insert(wlr::toPair(module, wlr::stringMap()));
		iter = wlr::Configure::s_configCache.find(module);
	}
	
	(*iter).second.insert(wlr::toPair(key, value));
    return true;
}

// map<module name, map<key, value> >
std::map<std::string, std::map<std::string, std::string> > 
wlr::Configure::s_configCache = std::map<std::string, std::map<std::string, std::string> >();


