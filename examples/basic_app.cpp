#include <orni/orni.hpp>

int main() {
    orni::HttpServer app;
    app.route("/", [](orni::Request& req, orni::Response& res) {
        res.send("Hello, World!");
    });
    app.run(1234);
}
