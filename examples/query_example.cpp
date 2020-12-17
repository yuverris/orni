#include "../src/orni_http_server.hpp"

int main() {
    orni::HttpServer app;
    app.route("/", [](orni::Request&& req, orni::Response&& res) {
            std::stringstream ss;
            auto eggs = req.Queries["eggs"];
            if (eggs.empty()) {
                ss << "Empty order :(";
            } else if (!eggs.empty()) {
                ss << "You ordered " << eggs << " total egg!";
            }
            res.send(ss.str());
            res.dump();
   });
   app.route("/user/:id", [](orni::Request&& req, orni::Response&& res) {
            res.send("you requested for User id " + req.Params["id"]);
            res.dump();
   });
   app.route("/user/", [](orni::Request&& req, orni::Response&& res) {
            res.send("user path");
            res.dump();
   });
   app.route("/user/special", [](orni::Request&& req, orni::Response&& res) {
            res.send("special user path");
            res.dump();
   });
   app.run(1234);
}
