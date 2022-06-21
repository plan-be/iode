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


class IodeExceptionRuntime : public IodeException
{
    std::string action;
    std::string reason;

public:
    IodeExceptionRuntime(const std::string& action, const std::string& reason) : IodeException(""), action(action), reason(reason) 
    {
        message = "Cannot performe " + action + ": " + reason;
    }
};


class IodeExceptionFunction : public IodeException
{
protected:
    std::string function_name;
    std::string prefix_arguments;
    bool first_arg;

public:
    IodeExceptionFunction(const std::string& function_name, const std::string action = "run") : IodeException(""), function_name(function_name), first_arg(true)
    {
        prefix_arguments = "";
        std::string message = "Cannot " + action + " " + function_name;
    }

    void add_argument(const std::string name, const std::string value)
    {
        if (first_arg) message += prefix_arguments + "Parameter Values:\n";
        message += name + ": " + value + "\n";
        first_arg = false;
    }
};


class IodeExceptionInvalidArguments : public IodeExceptionFunction
{
public:
    IodeExceptionInvalidArguments(const std::string& function_name, const std::string action = "run") : IodeExceptionFunction(function_name, action)
    {
        prefix_arguments = "Invalid ";
    }

    bool invalid_args() { return !first_arg; }
};


class IodeExceptionInitialization : public IodeExceptionFunction
{
public:
    IodeExceptionInitialization(const std::string& class_name) : IodeExceptionFunction(function_name, "create") {}
};
