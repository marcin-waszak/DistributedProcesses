#include "AdminServerConnection.h"
#include "Admin.h"

namespace po = boost::program_options;
using std::cout;
using std::endl;
using std::string;

int main(int argc, char* argv[]) {
    Admin admin;

    admin.GetArguments(argc, argv);
    admin.Connect();

    if (admin.IsInteractive())
        admin.CommandParser();
    else
        admin.BatchMode();

    return 0;
}
