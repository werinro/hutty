#ifndef __WLR__MAPS__H
#define __WLR__MAPS__H


#include <map>
#include <set>
#include <list>
#include <string.h>


namespace wlr
{

template<class K, class V>
inline std::pair<K, V> toPair(K k, V v)
{
	return std::pair<K, V>(k, v); 
}

inline std::map<std::string, std::string> stringMap()
{
	return std::map<std::string, std::string>();
}

template<class Key, class Value>
inline std::map<Key, Value> getMap(Key key, Value value)
{
	std::map<Key, Value> map;
	map.insert(toPair(key, value));
	return map;
}

inline std::map<std::string, std::string> stringMap(std::string key, std::string value)
{
    return getMap(key, value);
}

#define W_FOREACH_I(iterator, func) \
	int i_index = 0; \
    for (auto i_iter = iterator.begin(); i_iter != iterator.end(); i_iter++, i_index++) \
        func((*i_iter), i_index);

template<class T, class F>
inline void foreach(std::list<T> list, F func)
{
	W_FOREACH_I(list, func)
}

template<class V, class F>
inline void foreach(std::set<V> set, F func)
{
	W_FOREACH_I(set, func)
}

#undef W_FOREACH_I

template<class K, class V, class F>
inline void foreach(std::map<K, V> map, F func)
{
	for (auto iter = map.begin(); iter != map.end(); iter++) 
		func((*iter).first, (*iter).second);
}


}


#endif
