#include "AdminServerConnection.h"
#include "Admin.h"

namespace po = boost::program_options;
using std::cout;
using std::endl;
using std::string;

int main(int argc, char* argv[]) {
    // TODO: implement properly

    Admin admin;

    admin.GetArguments(argc, argv);
    admin.Connect();
    admin.CommandParser();

    return 0;
}
