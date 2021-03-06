#include "Server.h"

bool Log::Enable = true;

int main(int argc, char** argv) {
    Srv::Server server;

    server.GetArguments(argc, argv);

    if (server.ServerLoop())  {
        cout << "Closing server" << endl;
        // TODO: serialize state
        return 0;
    } else {
        cout << "Server run failed" << endl;
        return 1;
    }
}
