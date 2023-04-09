#ifndef Exception_INCLUDED
#define Exception_INCLUDED

#include <exception>
#include <string>


class Exception
    : public std::exception {
public:
    Exception();
    Exception(std::string const& message);
    char const* what() const noexcept override;
    std::string const& getMessage() const;

private:
    std::string const m_message;
};

#endif // Exception_INCLUDED
