#include <orni/orni.hpp>

int main() {
    orni::HttpServer app;
    app.route("/", [](orni::Request& req, orni::Response& res) {
        res.send("Hello, World!");
    });
    app.route("/redirect", [](orni::Request& req, orni::Response& res) {
        res.send("redirecting you to /target");
        res.Redirect("/target");
    });
    app.route("/target", [](orni::Request& req, orni::Response& res) {
        res.send("redirected successfully");
    });
    app.run(1234);  //  or any port default to 5000
}
