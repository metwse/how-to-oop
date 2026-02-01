#include "payload.hpp"

#include <iostream>

using std::cout, std::endl;


void Command::process() {
    cout << "Command: " << command_name << endl;
    process_arguments();
}

void LoginCommand::process_arguments() {
    cout << "  Arguments: [username: " << username
        << ", password: " << password << "]" << endl;
}

void JoinCommand::process_arguments() {
    cout << "  Arguments: [channel: " << channel << "]" << endl;
}

void LogoutCommand::process_arguments() {
    cout << "  Arguments: []" << endl;
}


void Message::process() {
    process_recipient();
    cout << content << endl;
}

void DirectMessage::process_recipient() {
    cout << "Direct message to " << username << ": ";
}

void GroupMessage::process_recipient() {
    cout << "Group message to " << channel << ": ";
}

void GlobalMessage::process_recipient() {
    cout << "Global message: ";
}
