#include <orni/orni.hpp>

int main() {
    orni::HttpServer app;
    app.route("/", [](orni::Request& req, orni::Response& res) {
        std::stringstream ss;
        auto eggs = req.Queries["eggs"];
        if (eggs.empty()) {
            ss << "Empty order :(";
        } else if (!eggs.empty()) {
            ss << "You ordered " << eggs << " total egg!";
        }
        res.send(ss.str());
    });
    app.route("/user/:id", [](orni::Request& req, orni::Response& res) {
        res.send("you requested for User id " + req.Params["id"]);
    });
    app.route("/user/", [](orni::Request& req, orni::Response& res) {
        res.send("user path");
    });
    app.route("/user/special", [](orni::Request& req, orni::Response& res) {
        res.send("special user path");
    });
    app.run(1234);
}
