#include "Admin.h"

#include "Server.h"
#include <iostream>
using std::cerr;
using std::cout;
using std::endl;

Admin::Admin(unique_ptr<Connection> connection, Server& server)
        : connection_(std::move(connection)),
        server_(server),
        closed_(false) {
    thread_ = make_unique<thread>(&Admin::Loop, this);
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
    string msg = connection_->RecvMsg();
    cout << "got command: " << msg << endl;
    if (msg == "GET_WORKERS") {
        auto wids = server_.GetWorkerIDs();
        string resp;
        if (wids.empty()) {
            resp = "<empty>\n";
        } else {
            ostringstream ss;
            for (auto& w : wids) {
                ss << w << " -> "
                   << server_.GetWorker(w)->GetAddress() << endl;
            }
            resp = ss.str();
        }
        cout << "responding: " << resp << endl;
        connection_->SendMsg(resp);
    } else if (msg == "GET_IMAGES_LIST") {
        if (server_.GetProcessImages().empty()) {
            string resp = "<empty>";
            connection_->SendMsg(resp);
            cout << "responding:" << resp << endl;
        } else {
            ostringstream oss;
            for (const ProcessImage& pi : server_.GetProcessImages()) {
                oss << pi.GetPath() << endl;
            }
            cout << "responding:\n" << oss.str() << endl;
            connection_->SendMsg(oss.str());
        }
    } else if (msg == "UPLOAD_IMAGE") {
        string name = connection_->RecvMsg();
        fs::path filePath = server_.GetImagesPath() / name;
        ProcessImage pi = connection_->RecvProcessImage(filePath);
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
        // TODO: file save errors
        connection_->SendMsg("OK");
    } else if (msg == "DELETE_IMAGE") {
        string name = connection_->RecvMsg();
        fs::path filePath = server_.GetImagesPath() / name;
        remove(filePath);
        server_.RemoveProcessImage(filePath);
        connection_->SendMsg("OK");
    } else if (msg == "GET_WORKERS_IMAGES") {
        auto wids = server_.GetWorkerIDs();
        ostringstream ss("");
        for (auto& w : wids) {
            auto worker = server_.GetWorker(w);
            ss << w << " -> "
                << worker->GetAddress() << endl;
            string imagesList = worker->ListImages();
            if (imagesList == "")
                imagesList = "<empty>";
            ss << imagesList << endl;
        }
        string resp = ss.str();
        if (resp == "")
            resp = "<empty>";
        cout << "responding: " << resp << endl;
        connection_->SendMsg(resp);
    } else if (msg == "UPLOAD_IMAGE_WORKER") {
        string name = connection_->RecvMsg();
        string worker_id_str = connection_->RecvMsg();
        try {
            int worker_id = boost::lexical_cast<int>(worker_id_str);
            auto worker = server_.GetWorker(worker_id);
            bool found = false;
            for (auto &p : server_.GetProcessImages()) {
                if (p.GetPath().filename() == name) {
                    string resp = worker->UploadImage(p);
                    connection_->SendMsg(resp);
                    found = true;
                    break;
                }
            }
            if (!found)
                connection_->SendMsg("Such image does not exist on server");
        } catch (ServerExeption) {
            connection_->SendMsg("Such worker does not exist.");
        } catch (boost::bad_lexical_cast) {
            connection_->SendMsg("Wrong worker id.");
        }
    } else if (msg == "DELETE_IMAGE_WORKER") {
        string name = connection_->RecvMsg();
        string worker_id_str = connection_->RecvMsg();
        try {
            int worker_id = boost::lexical_cast<int>(worker_id_str);
            auto worker = server_.GetWorker(worker_id);
            bool found = false;
            for (auto &p : server_.GetProcessImages()) {
                if (p.GetPath().filename() == name) {
                    string resp = worker->DeleteImage(p);
                    connection_->SendMsg(resp);
                    found = true;
                    break;
                }
            }
            if (!found)
                connection_->SendMsg("Such image does not exist on server");
        } catch (ServerExeption) {
            connection_->SendMsg("Such worker does not exist.");
        } catch (boost::bad_lexical_cast) {
            connection_->SendMsg("Wrong worker id.");
        }
    } else if (msg == "RUN_NOW") {
        string name = connection_->RecvMsg();
        string worker_id_str = connection_->RecvMsg();
        try {
            int worker_id = boost::lexical_cast<int>(worker_id_str);
            auto worker = server_.GetWorker(worker_id);
            string resp = worker->RunNow(name);
            connection_->SendMsg(resp);
        } catch (ServerExeption) {
            connection_->SendMsg("Such worker does not exist.");
        } catch (boost::bad_lexical_cast) {
            connection_->SendMsg("Wrong worker id.");
        }
    } else if (msg == "STOP_NOW") {
        string name = connection_->RecvMsg();
        string worker_id_str = connection_->RecvMsg();
        try {
            int worker_id = boost::lexical_cast<int>(worker_id_str);
            auto worker = server_.GetWorker(worker_id);
            string resp = worker->StopNow(name);
            connection_->SendMsg(resp);
        } catch (ServerExeption) {
            connection_->SendMsg("Such worker does not exist.");
        } catch (boost::bad_lexical_cast) {
            connection_->SendMsg("Wrong worker id.");
        }
    } else if (msg == "CLOSE") {
        closed_ = true;
        return false;
    } else {
        cout << "unknown command, ignoring" << endl;
    }
    return true;
}

bool Admin::Closed() {
    return closed_;
}
