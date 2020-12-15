#include "orni_http_server.hpp"

int main() {
    orni::HttpServer app;
    app.setPort(1235);
    app.route("/", [&](orni::Request req, orni::Response res) {
            res.set("Content-Type", "text/html");
            res.send("<h1>Hello from orni!</h1>");
            res.dump();
    });
    app.run();
}
