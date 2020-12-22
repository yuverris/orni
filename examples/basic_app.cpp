#include <orni/orni.hpp>

int main() {
    orni::HttpServer app;
    app.Get("/", [](orni::Request& req, orni::Response& res) {
        res.Send("Hello, World!");
    });
    app.run(1234);
}
