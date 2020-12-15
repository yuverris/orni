/*
 * Copyright © 2020-2021 Dammi
 * License: AGPLv3
 * */
#include "orni_http_server.hpp"

int main() {
    orni::HttpServer app;
    app.route("/", [&](orni::Request&& req, orni::Response&& res) {
            res.set("Content-Type", "text/html");
            res.send("<h1>Hello from orni!</h1>");
            res.dump();
    });
    app.route("/test/:id/", [&](orni::Request&& req, orni::Response&& res) {
            std::string prm = req.Params["id"];
            res.send(prm);
            res.dump();
    });
    app.run(1234);
}
