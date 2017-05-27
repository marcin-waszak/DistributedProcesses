#include "Admin.h"

#include "Server.h"

#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

Admin::Admin(int connect_id, Server& srv)
        : connection_(connect_id),
        server_(srv),
        closed_(false) {
    thread_ = make_unique<thread>(thread(&Admin::Loop, this));
}

Admin::~Admin() {
    assert(closed_);
    thread_->join();
}

void Admin::Loop() {
    for (;;) {
        try {
            if (!ExecCmd())
                break;
        } catch (ConnectionException) {
            cerr << "Admin disconnected" << endl;
            closed_ = true;
            break;
        }
    }
}

bool Admin::ExecCmd() {
    string msg = connection_.RecvMsg();
    cout << "got command: " << msg << endl;
    if (msg == "GET_WORKERS") {
        // workers not implemented yet
        string resp = "0";
        cout << "responding: " << resp << endl;
        connection_.SendMsg(resp);
    } else if (msg == "GET_IMAGES_LIST") {
        if (server_.GetProcessImages().empty()) {
            string resp = "<empty>";
            connection_.SendMsg(resp);
            cout << "responding:" << resp << endl;
        } else {
            ostringstream oss;
            for (const ProcessImage& pi : server_.GetProcessImages()) {
                oss << pi.GetPath() << endl;
            }
            cout << "responding:\n" << oss.str() << endl;
            connection_.SendMsg(oss.str());
        }
    } else if (msg == "UPLOAD_IMAGE") {
        string name = connection_.RecvMsg();
        fs::path filePath = server_.GetImagesPath() / name;
        ProcessImage pi = connection_.RecvProcessImage(filePath);
        cout << "image saved: " << filePath << endl;
        bool found = false;
        for (auto& p : server_.GetProcessImages()) {
            if (p.GetPath() == pi.GetPath()) {
                found = true;
                break;
            }
        }
        if (!found)
            server_.AddProcessImage(pi);
    } else if (msg == "CLOSE") {
        closed_ = true;
        return false;
    } else {
        cout << "unknown command, ignoring" << endl;
    }

    // false on connection close
    return true;
}

bool Admin::Closed() {
    return closed_;
}
