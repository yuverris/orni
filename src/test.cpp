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
    app.route("/test/redirect/", [&](orni::Request&& req, orni::Response&& res) {
            res.redirect("/");
            res.dump();
    });
    app.route("/test/query/", [&](orni::Request&& req, orni::Response&& res) {
            std::stringstream ss;
            for (auto& [key, val]: req.Queries) {
                ss << key << " " << val << '\n';
            }
            res.send(ss.str());
            res.dump();
    });
    app.run(1234);
}
