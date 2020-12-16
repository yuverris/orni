#include "orni_http_server.hpp"

int main() {
    orni::HttpServer app;
    app.route("/path/:id/", [&](orni::Request&& req, orni::Response&& res) {
            res.send(req.Params["id"] + " " + req.Queries["name"]);
            res.dump();
    });
    app.route("/", [&](orni::Request&& req, orni::Response&& res) {
            res.send("hello");
            res.dump();
    });
    app.run(1234);
}
