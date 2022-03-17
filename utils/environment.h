#ifndef __WLR__ENVIRONMENT__H
#define __WLR__ENVIRONMENT__H


#include <stdio.h>


#ifndef __linux__
#else
#include <sys/sysinfo.h>
#include <unistd.h>
#endif



// The number of processors configured
#define ENV_CORES &wlr::Environment::SYS_CORSE
// The number of processors currently online (available)
#define ENV_OL_CORES &wlr::Environment::SYS_ONLINE_CORSE
// Size of a memory page in bytes
#define ENV_M_PAGE_SIZE &wlr::Environment::SYS_PAGE_SIZE
// The number of pages of physical memory
#define ENV_M_PAGE_TOTAL &wlr::Environment::SYS_PAGE_TOTAL
// The number of currently available pages of physical memory
#define ENV_M_PAGE_FREE &wlr::Environment::SYS_PAGE_AVAILABLE
// The physical  memory total size
#define ENV_MEMORY_TOTAL &wlr::Environment::SYS_MEMORY_TOTAL
// The physical  memory free available size.
#define ENV_MEMORY_FREE &wlr::Environment::SYS_MEMORY_FREE
// Total system operation time
#define ENV_RUNNING_TIME &wlr::Environment::SYS_RUNNING_TIME
// Total number of system running processes
#define ENV_PROCESS_TOTAL &wlr::Environment::SYS_PROCESS_TOTAL


namespace wlr
{


class Environment
{
public:
	
	template<class Value>
	class Option
	{
		friend void wlr::Environment::load();
	public:
		explicit Option(Value value) : m_value(value) {}
		inline Value get() { return this->m_value; }
	private:
		inline void set(Value value) { this->m_value = value; }
		Value m_value;
	};

	template<class Value>
	static Value get(wlr::Environment::Option<Value> *option) {
		load();
		return option->get();
	}

    static wlr::Environment::Option<int> SYS_CORSE;
    static wlr::Environment::Option<int> SYS_ONLINE_CORSE;
    static wlr::Environment::Option<int> SYS_PAGE_SIZE;
    static wlr::Environment::Option<size_t> SYS_PAGE_TOTAL;
    static wlr::Environment::Option<size_t> SYS_PAGE_AVAILABLE;
    static wlr::Environment::Option<size_t> SYS_MEMORY_TOTAL;
    static wlr::Environment::Option<size_t> SYS_MEMORY_FREE;
    static wlr::Environment::Option<size_t> SYS_RUNNING_TIME;
    static wlr::Environment::Option<int> SYS_PROCESS_TOTAL;

private:
	static void load() {

#define TO_INT(t) static_cast<int>(t)
#define TO_SIZE_T(t) static_cast<size_t>(t)

#ifndef __linux__
#else
		if (SYS_CORSE.get() < 1) {
			SYS_CORSE.set(TO_INT(sysconf(_SC_NPROCESSORS_CONF)));
			SYS_ONLINE_CORSE.set(TO_INT(sysconf(_SC_NPROCESSORS_ONLN)));
			SYS_PAGE_SIZE.set(TO_INT(sysconf(_SC_PAGESIZE)));
			SYS_PAGE_TOTAL.set(TO_SIZE_T(sysconf(_SC_PHYS_PAGES)));
		}

		SYS_PAGE_AVAILABLE.set(TO_SIZE_T(sysconf(_SC_AVPHYS_PAGES)));
		struct sysinfo sys_info;
		int r = sysinfo(&sys_info);
		if (r == 0) 
		{
			SYS_MEMORY_TOTAL.set(TO_SIZE_T(sys_info.totalram));
			SYS_MEMORY_FREE.set(TO_SIZE_T(sys_info.freeram));
			SYS_RUNNING_TIME.set(TO_SIZE_T(sys_info.uptime));
			SYS_PROCESS_TOTAL.set(TO_INT(sys_info.procs));
			// printf("sys_info.mem_unit = %d\n", sys_info.mem_unit);
		}
		else 
		{
			printf("envitonment load sysinfo error.\n");
		}
#endif
#undef TO_SIZR_T
#undef TO_INT

	}
private:
	Environment();
};

wlr::Environment::Option<int> wlr::Environment::SYS_CORSE = wlr::Environment::Option<int>(0);
wlr::Environment::Option<int> wlr::Environment::SYS_ONLINE_CORSE = wlr::Environment::Option<int>(0);
wlr::Environment::Option<int> wlr::Environment::SYS_PAGE_SIZE = wlr::Environment::Option<int>(4096);
wlr::Environment::Option<size_t> wlr::Environment::SYS_PAGE_TOTAL = wlr::Environment::Option<size_t>(0);
wlr::Environment::Option<size_t> wlr::Environment::SYS_PAGE_AVAILABLE = wlr::Environment::Option<size_t>(0);
wlr::Environment::Option<size_t> wlr::Environment::SYS_MEMORY_TOTAL = wlr::Environment::Option<size_t>(0);
wlr::Environment::Option<size_t> wlr::Environment::SYS_MEMORY_FREE = wlr::Environment::Option<size_t>(0);
wlr::Environment::Option<size_t> wlr::Environment::SYS_RUNNING_TIME = wlr::Environment::Option<size_t>(1);
wlr::Environment::Option<int> wlr::Environment::SYS_PROCESS_TOTAL = wlr::Environment::Option<int>(1);

}


#endif
