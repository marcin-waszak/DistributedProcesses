#include "Server.h"


int main(int argc, char** argv) {
    Server server;

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
