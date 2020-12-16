#include "../src/orni_http_server.hpp"

int main() {
    orni::HttpServer app;
    app.route("/", [](orni::Request&& req, orni::Response&& res) {
            res.send("Hello, World!");
            res.dump();
    });
    app.route("/redirect", [](orni::Request&& req, orni::Response&& res) {
            res.send("redirecting you to /target");
            res.redirect("/target");
            res.dump();
    });
    app.route("/target", [](orni::Request&& req, orni::Response&& res) {
            res.send("redirected successfully");
            res.dump();
    });
    app.run(1234); //  or any port default to 5000
}
