#ifndef __WLR__EXCEPTION__H
#define __WLR__EXCEPTION__H

#include <string.h>
#include <sstream>
#include <vector>
#include <stdio.h>


#ifndef PRINT_STACK_DEPTH
#define PRINT_STACK_DEPTH 50
#endif
#define W_THROW(e, msg) \
	throw wlr::e(msg, wlr::Exception::stackVector(), __FILE__, __LINE__)


namespace wlr
{


class Exception
{
public:
	Exception(std::string message, std::vector<std::string> stack_vector = std::vector<std::string>());
	virtual ~Exception();

	std::string message() {return this->m_message;}
	void setMessage(std::string message) {this->m_message = message;}
	std::vector<std::string> stack() { return this->m_stack_vector; }
	std::string stackString(int depth = PRINT_STACK_DEPTH);
	static std::vector<std::string> stackVector(int depth = PRINT_STACK_DEPTH);
private:
	std::string m_message;
	std::vector<std::string> m_stack_vector;
};

#define EXCEPTION_DECLARATION(name) \
	class name : public Exception \
	{ \
	public: \
		name(std::string message, std::vector<std::string> stack_vector, std::string fileName, int fileLine); \
	};

EXCEPTION_DECLARATION(NullPointerException)
EXCEPTION_DECLARATION(IOException)
EXCEPTION_DECLARATION(SocketException)
EXCEPTION_DECLARATION(ClassCastException)
EXCEPTION_DECLARATION(ThreadException)

}

#endif
