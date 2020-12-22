#include <orni/orni.hpp>

int main() {
    orni::HttpServer app;
    app.Get("/", [](orni::Request& req, orni::Response& res) {
        res.Send("Hello, World!");
    });
    app.Get("/redirect", [](orni::Request& req, orni::Response& res) {
        res.Send("redirecting you to /target");
        res.Redirect("/target");
    });
    app.Get("/target", [](orni::Request& req, orni::Response& res) {
        res.Send("redirected successfully");
    });
    app.run(1234);  //  or any port default to 5000
}
