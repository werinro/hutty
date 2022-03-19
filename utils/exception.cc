#include "exception.h"
#include <execinfo.h>
#include <cxxabi.h>


wlr::Exception::Exception(std::string message, std::vector<std::string> stack_vector)
	: m_message(message)
	, m_stack_vector(stack_vector)
{}

std::string wlr::Exception::stackString(int depth)
{
	std::string str;
	for (auto iter = this->m_stack_vector.begin(); iter != this->m_stack_vector.end(); iter++) {
		str.append((*iter)).append("\n");
	}
	return str;
}

std::vector<std::string> wlr::Exception::stackVector(int depth)
{
    void* buffer[depth];
	int len = backtrace(buffer, depth);
    char** buf =  backtrace_symbols(buffer, len);

	std::vector<std::string> stack_vector;
    for (int i = 0; i < len; i++) {
        int index = 0;
        std::string str;
        char temp[1024];
        char address[128];
        int begin0 = false, index0 = 0;
        int begin1 = false, index1 = 0;
        for (; index < 1023; index++) {
            if (buf[i][index] == '\0') break;
            if (!begin1) {
                if (buf[i][index] == '+') {
                    begin0 = false;
                    temp[index0++] = '\0';
                    continue;
                } else if (begin0) {
                    temp[index0++] = buf[i][index];
                    continue;
                } else if (buf[i][index] == '(') {
                    begin0 = true;
                    continue;
                }
            }
		
			if (!begin0) {
                if (buf[i][index] == ']') {
                    address[index1++] = ']';
                    address[index1++] = '\0';
                    break;
                } else if (begin1 && index1 < 126) {
                    address[index1++] = buf[i][index];
                } else if (buf[i][index] == '[') {
                    address[index1++] = '[';
                    begin1 = true;
                }
            }
        }

        char* func_name = abi::__cxa_demangle(temp, NULL, NULL, NULL);
        if (func_name) {
            str.append(address, index1 -1).append(" ").append(func_name);
            free(func_name);
        } else {
            str.append(buf[i], index + 1);
        }

        stack_vector.push_back(str);
    }

    free(buf);
	return stack_vector;
}

wlr::Exception::~Exception() {}


#define W_SET_M(msg) \
    std::stringstream ss; \
	ss << msg << " [file(" <<  fileName << ")-line(" << fileLine << ")]- " << message; \
	this->setMessage(ss.str());

#define W_DEFINITION(name, msg) \
	wlr::name::name(std::string message, std::vector<std::string> stack_vector, std::string fileName, int fileLine) \
		: Exception(message, stack_vector) \
	{ \
	    W_SET_M(#msg) \
	}

W_DEFINITION(NullPointerException, null pointer)
W_DEFINITION(IOException, io error)
W_DEFINITION(SocketException, socket error)
W_DEFINITION(ClassCastException, class cast error)
W_DEFINITION(ThreadException, thread error)

#undef W_SET_M
#undef W_DEFINITION

