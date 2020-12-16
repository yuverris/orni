#include "../src/orni_http_server.hpp"

int main() {
    orni::HttpServer app;
    app.setNotFound([](orni::Request&& req, orni::Response&& res) {
            res.send("my custom 404 page");
            res.dump();
    });  //  for habdling 404 errors
    app.setServerError([](orni::Request&& req, orni::Response&& res) {
            res.send("my custom 500 page");
            res.dump();
    });  //  for habdling 500 swrver errors
    app.run(1234); //  or any port default to 5000
}
