#include "spglib.hpp"

#include <stdexcept>

extern "C" {
#include <spglib.h>
}

using namespace Spglib;

class NotImplemented : public std::logic_error {
public:
	NotImplemented() : std::logic_error("Function not yet implemented"){};
};

const char* Spglib::SpglibError::what() const noexcept {
	return errorMsg.c_str();
}
Spglib::SpglibError::SpglibError(
        std::string&& message) : errorMsg{message} {
}
