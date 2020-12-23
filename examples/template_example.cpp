#include <orni/orni.hpp>

int main() {
    orni::HttpServer app;
    app.Get("/", [](orni::Request&, orni::Response& res) {
        res.RenderTemplate("<h1>{{ text }}</h1>", {{"text", "Hello, World!"}});
    });
    app.run();
}
