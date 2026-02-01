/**
 * @file payload.hpp
 * @brief Payload implementations.
 */

#ifndef PAYLOAD_HPP
#define PAYLOAD_HPP


#include <string>


class Payload {
public:
    virtual void process() = 0;

    virtual ~Payload() = default;
};


/* Command base class ------------------------------------------------------ */
class Command : public Payload {
public:
    Command(const char *command_name_)
        : command_name { command_name_ } {};

    void process() override;

    virtual ~Command() = default;

private:
    virtual void process_arguments() = 0;

    std::string command_name;
};

/* Command types ----------------------------------------------------------- */
class LoginCommand : public Command {
public:
    LoginCommand(const char *username_, const char *password_)
        : Command { "login" }, username { username_ }, password { password_ } {}

private:
    void process_arguments() override;

    std::string username;
    std::string password;
};

class JoinCommand : public Command {
public:
    JoinCommand(const char *channel_)
        : Command { "join" }, channel { channel_ } {}

private:
    void process_arguments() override;

    std::string channel;
};

class LogoutCommand : public Command {
public:
    LogoutCommand()
        : Command { "logout" } {}

private:
    void process_arguments() override;
};


/* Message base class ------------------------------------------------------ */
class Message : public Payload {
public:
    Message(const char *content_)
        : content { content_ } {}

    void process() override;

private:
    virtual void process_recipient() = 0;

    std::string content;
};

/* Message types ----------------------------------------------------------- */
class DirectMessage : public Message {
public:
    DirectMessage(const char *content_, const char *username_)
        : Message { content_ }, username { username_ } {}

private:
    void process_recipient() override;

    std::string username;
};

class GroupMessage : public Message {
public:
    GroupMessage(const char *content_, const char *channel_)
        : Message { content_ }, channel { channel_ } {}

private:
    void process_recipient() override;

    std::string channel;
};

class GlobalMessage : public Message {
public:
    GlobalMessage(const char *content_)
        : Message { content_ } {}

private:
    void process_recipient() override;
};


#endif
