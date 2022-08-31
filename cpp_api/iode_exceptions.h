#pragma once
#include <string>
#include <map>
#include <stdexcept>


class IodeException : public std::exception
{
protected:
    std::string message;

public:
    explicit IodeException(const std::string& message) : message(message) {}
    explicit IodeException(const char* message) : message(message) {}
    virtual ~IodeException() noexcept {}
    virtual const char* what() const noexcept { return message.c_str(); }
};


class IodeExceptionFunction : public IodeException
{
protected:
    std::string reason;
    std::string prefix_arguments;
    bool first_arg;

public:
    IodeExceptionFunction(const std::string& description, const std::string& reason = "", 
                          const std::string& prefix_arguments = "- ") : 
        IodeException(""), prefix_arguments(prefix_arguments), first_arg(true)
    {
        message = description + "\n";
        set_reason(reason);
    }

    void set_reason(const std::string& reason)
    {
        if(!reason.empty()) message += "Reason: " + reason + "\n";
    }

    void add_argument(const std::string& name, const std::string& value)
    {
        if(first_arg) message += "Parameter Values:\n";
        message += prefix_arguments + name + ": " + value + "\n";
        first_arg = false;
    }
};


class IodeExceptionInvalidArguments : public IodeExceptionFunction
{
public:
    IodeExceptionInvalidArguments(const std::string& description, 
                                  const std::string& reason = "One or several arguments are invalid!")
        : IodeExceptionFunction(description, reason, "- Invalid argument -> ") {}

    bool invalid_args() { return !first_arg; }
};


class IodeExceptionInitialization : public IodeExceptionFunction
{
public:
    IodeExceptionInitialization(const std::string& class_name, const std::string& reason = "") 
        : IodeExceptionFunction("Cannot create new " + class_name, reason, "- ") {}
};
