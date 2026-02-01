/**
 * @file payload.hpp
 * @brief Command payload implementatoins.
 */

#ifndef PAYLOAD_HPP
#define PAYLOAD_HPP


#include "string.hpp"


class LoginCommand {
private:
    String username;
    String password;

public:
    LoginCommand(const char *i_username, const char *i_password)
        : username { i_username }, password { i_password } {}

    void process();
};

class JoinCommand {
private:
    String channel;

public:
    JoinCommand(const char *i_channel)
        : channel { i_channel } {}

    void process();
};

class LogoutCommand {
public:
    LogoutCommand() = default;

    void process();
};


/**
 * @brief One gigantic function for handling a payload.
 * @param raw One line, representing a payload
 */
void handle_command_payload(char *raw);


#endif
