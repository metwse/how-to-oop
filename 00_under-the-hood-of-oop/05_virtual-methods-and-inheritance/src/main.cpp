#include "payload.hpp"


int main() {
    Payload *payloads[6];
    payloads[0] = new LoginCommand { "alice", "pass123" };
    payloads[1] = new JoinCommand { "general" };
    payloads[2] = new LogoutCommand {};

    payloads[3] = new DirectMessage { "How are you doing?", "bob" };
    payloads[4] = new GroupMessage { "Server maintainence tonight", "announcements" };
    payloads[5] = new GlobalMessage { "Hello, world!" };

    for (int i = 0; i < 6; i++)
        payloads[i]->process();

    for (int i = 0; i < 6; i++)
        delete payloads[i];
}
