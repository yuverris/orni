#include <orni/orni.hpp>

int main() {
    orni::HttpServer app;
    app.route("/", [](orni::Request&& req, orni::Response&& res) {
        res.send("Hello, World!");
        res.dump();
    });
    app.run(1234);
}
