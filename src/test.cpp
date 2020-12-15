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
    app.route("/about/", [&](orni::Request&& req, orni::Response&& res) {
            res.set("Content-Type", "text/html");
            res.send("<h1>About route</h1>");
            res.dump();
    });
    app.route("/pages/", [&](orni::Request&& req, orni::Response&& res) {
            res.set("Content-Type", "text/html");
            res.send("<h1>Pages route</h1>");
            res.dump();
    });
    app.alias("/pages", "/pages/");
    app.alias("/about", "/about/");
    app.run(1234);
}
