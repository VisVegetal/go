#ifndef GO_EXCEPTIONS_HPP
#define GO_EXCEPTIONS_HPP

#include <exception>
#include <string>

class CustomGoException : public std::exception {
private:
    std::string message;

public:
    CustomGoException(const std::string& msg) : message(msg) {}

    virtual const char* what() const noexcept override {
        return message.c_str();
    }
    virtual ~CustomGoException() noexcept = default;
};
class InvalidMoveException : public CustomGoException {
public:
    InvalidMoveException(const std::string& reason)
        : CustomGoException("Mutare ilegala: " + reason) {}
};

class InvalidInputException : public CustomGoException {
public:
    InvalidInputException(const std::string& reason)
        : CustomGoException("Input invalid: " + reason) {}
};

class PositionOccupiedException : public InvalidMoveException {
public:
    PositionOccupiedException()
        : InvalidMoveException("Pozitia aleasa este deja ocupata.") {}
};

class GameSetupException : public CustomGoException {
public:
    GameSetupException(const std::string& reason)
        : CustomGoException("Eroare de configurare a jocului: " + reason) {}
};



#endif
