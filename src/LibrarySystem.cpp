#include "LibraryException.h"

LibraryException::LibraryException(const std::string& msg) : message(msg) {
}

const char* LibraryException::what() const noexcept {
    return message.c_str();
}
