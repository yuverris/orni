#include <orni/orni.hpp>

int main() {
    orni::HttpServer app;
    app.Get("/", [](orni::Request& req, orni::Response& res) {
        std::stringstream ss;
        auto eggs = req.Queries["eggs"];
        if (eggs.empty()) {
            ss << "Empty order :(";
        } else if (!eggs.empty()) {
            ss << "You ordered " << eggs << " total egg!";
        }
        res.Send(ss.str());
    });
    app.Get("/user/(\\d+)", [](orni::Request& req, orni::Response& res) {
        res.Send("you requested for User id " + req.Params[1]);
    });
    app.Get("/user/", [](orni::Request& req, orni::Response& res) {
        res.Send("user path");
    });
    app.Get("/user/special", [](orni::Request& req, orni::Response& res) {
        res.Send("special user path");
    });
    app.Get("/user/(\\w+)/(\\w+)", [](orni::Request& req, orni::Response& res) {
        res.Send("you requested for " + req.Params[1] + "'s " + req.Params[2]);
    });
    app.run(1234);
}
