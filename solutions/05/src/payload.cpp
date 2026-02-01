#include "payload.hpp"

#include <cstring>
#include <iostream>

using std::cout, std::endl;


void LoginCommand::process() {
    cout << "Command: login\n  Arguments: [username: " << username
         << ", password:" << password << "]" << endl;
}

void JoinCommand::process() {
    cout << "Command: join\n  Arguments: [channel: " << channel << "]" << endl;
}

void LogoutCommand::process() {
    cout << "Command: logout\n  Arguments: []" << endl;
}

void handle_command_payload(char *raw) {
    if (raw[0] != '/') {
        cout << "Skipping non-command" << endl;
    }

    raw += 1;
    char *rest = raw;

    char *cmd = strtok_r(raw, " ", &rest);

    if (strcmp(cmd, "login") == 0) {
        char *username = strtok_r(rest, " ", &rest);
        char *password = strtok_r(rest, " ", &rest);

        LoginCommand cmd(username, password);
        cmd.process();  // recall syntactic sugar
    } else if (strcmp(cmd, "join") == 0) {
        char *channel = strtok_r(rest, " ", &rest);

        JoinCommand cmd(channel);
        cmd.process();
    } else if (strcmp(cmd, "logout") == 0) {
        LogoutCommand cmd;
        cmd.process();
    } else {
        cout << "Unknown command!" << endl;
    }

    cout << "\n";
}
