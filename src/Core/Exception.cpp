#include "Core/Exception.hpp"


Exception::Exception()
    : m_message() {
}

Exception::Exception(std::string const& message)
    : m_message(message) {
}

char const* Exception::what() const noexcept {
    return m_message.c_str();
}

std::string const& Exception::getMessage() const {
    return m_message;
}
