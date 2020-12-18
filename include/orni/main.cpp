#include "orni.hpp"

int main() {
    orni::HttpServer app;
    app.route("/", [&](orni::Request&& req, orni::Response&& res) {
        res.send("hello !");
        res.dump();
    });
    app.run();
}
